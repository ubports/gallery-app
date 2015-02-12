/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Content 0.1
import "Components"
import "OrganicView"
import "Utility"
import "../js/Gallery.js" as Gallery
import "../js/GalleryUtility.js" as GalleryUtility

/*!
  PhotosOverview show all the photos of the collection in chronological order in one big grid
*/
Page {
    id: photosOverview

    /// Emitted when a photos was clicked
    /// @param mediaSource is the photo object
    /// @param thumbnailRect is the rect, the photo thumbnails is shown in
    signal mediaSourcePressed(var mediaSource, var thumbnailRect)

    /// The to show in this view
    property MediaCollectionModel model

    /// True if in the selection mode
    property alias inSelectionMode: d.inSelectionMode

    property alias selection: d.selection

    /// Quit selection mode, and unselect all photos
    function leaveSelectionMode() {
        d.selection.unselectAll();
        d.selection.inSelectionMode = false;
    }

    function positionViewAtSelected(index) {
        photosGrid.positionViewAtIndex(index, GridView.Center);
    }

    property string pageTitle

    tools: inSelectionMode ? d.selectionTools : d.overviewTools

    Image {
        anchors.fill: parent

        source: "../img/background-paper.png"
        fillMode: Image.Tile
    }

    MediaGrid {
        id: photosGrid

        anchors.fill: parent
        model: photosOverview.model
        selection: d.selection
    }

    Component {
        id: deleteDialog
        DeleteDialog {
            title: {
                if (d.selection.selectedMediaCount === 1) {
                    if (d.selection.selectedVideosCount === 0) 
                        return i18n.tr("Delete 1 photo");
                    else
                        return i18n.tr("Delete 1 video");
                } else {
                    if (d.selection.selectedVideosCount === 0)
                        return i18n.tr("Delete %1 photos").arg(d.selection.selectedPhotosCount);
                    else if (d.selection.selectedPhotosCount === 0)
                        return i18n.tr("Delete %1 videos").arg(d.selection.selectedVideosCount);
                    else {
                        if (d.selection.selectedVideosCount === 1 && d.selection.selectedPhotosCount !== 1)
                            return i18n.tr("Delete %1 photos and 1 video").arg(d.selection.selectedPhotosCount);
                        else if (d.selection.selectedPhotosCount === 1 && d.selection.selectedVideosCount !== 1)
                            return i18n.tr("Delete 1 photo and %1 videos").arg(d.selection.selectedVideosCount);
                        else if (d.selection.selectedVideosCount === 1 && d.selection.selectedPhotosCount === 1)
                            return i18n.tr("Delete 1 photo and 1 video");
                        else
                            return i18n.tr("Delete %1 photos and %2 videos").arg(d.selection.selectedPhotosCount)
                                                                            .arg(d.selection.selectedVideosCount);
                    }
                }
            }
            onDeleteClicked: {
                d.selection.model.destroySelectedMedia();
                photosOverview.leaveSelectionMode();
            }
        }
    }

    property int __pickerContentHeight: height - units.gu(20)
    property PopupAlbumPicker __albumPicker
    Connections {
        target: __albumPicker
        onAlbumPicked: {
            album.addSelectedMediaSources(d.selection.model.selectedMedias);
            photosOverview.leaveSelectionMode();
        }
    }

    Item {
        id: d

        property bool inSelectionMode: selection.inSelectionMode
        property SelectionState selection: SelectionState {
            model: photosOverview.model
        }

        property Item overviewTools: PhotosToolbarActions {
            selection: d.selection
        }

        property Item selectionTools: SelectionToolbarAction {
            selection: d.selection

            onCancelClicked: {
                photosOverview.leaveSelectionMode();
            }
            onAddClicked: {
                __albumPicker = PopupUtils.open(Qt.resolvedUrl("Components/PopupAlbumPicker.qml"),
                                                null,
                                                {contentHeight: photosOverview.__pickerContentHeight});
            }
            onDeleteClicked: {
                PopupUtils.open(deleteDialog, null);
            }

            onShareClicked: {
                overview.pushPage(sharePicker)
                sharePicker.visible = true;
            }
        }
    }

    Component {
        id: contentItemComp
        ContentItem {}
    }

    Page {
        id: sharePicker
        visible: false
        title: i18n.tr("Share to")

        ContentPeerPicker {
            objectName: "sharePickerPhotos"
            showTitle: false
            anchors.fill: parent
            contentType: d.selection.mediaType === MediaSource.Video ? ContentType.Videos : ContentType.Pictures
            handler: ContentHandler.Share

            onPeerSelected: {
                overview.popPage();
                sharePicker.visible = false;

                var curTransfer = peer.request();
                if (curTransfer.state === ContentTransfer.InProgress)
                {
                    curTransfer.items = d.selection.model.selectedMediasQML.map(function(data) {
                        return contentItemComp.createObject(parent, {"url": data.path});
                    });
                    curTransfer.state = ContentTransfer.Charged;
                }
            }
            onCancelPressed: {
                overview.popPage();
                sharePicker.visible = false;
            }
        }
    }
}

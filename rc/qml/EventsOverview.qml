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
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Gallery 1.0
import Ubuntu.Content 0.1
import "Components"
import "OrganicView"
import "Utility"
import "../js/GalleryUtility.js" as GalleryUtility

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
OrganicView {
    id: organicEventView

    /// True if in the selection mode
    property alias inSelectionMode: select.inSelectionMode

    /// Quit selection mode, and unselect all photos
    function leaveSelectionMode() {
        selection.unselectAll()
        selection.inSelectionMode = false
    }

    selection: SelectionState {
        id: select
    }

    model: EventCollectionModel {
    }

    delegate: OrganicMediaList {
        id: photosList
        objectName: "organicEventItem" + index

        Connections {
            target: organicEventView
            onPositionEventsAtBeginning: {
                photosList.positionViewAtBeginning();
            }
        }

        width: organicEventView.width
        event: model.event
        selection: organicEventView.selection

        onPressed: {
            var rect = GalleryUtility.translateRect(thumbnailRect, photosList,
                                                    organicEventView);
            organicEventView.mediaSourcePressed(mediaSource, rect);
        }
    }

    Component {
        id: deleteDialog
        DeleteDialog {
            title: {
                if (organicEventView.selection.selectedMediaCount === 1) {
                    if (organicEventView.selection.selectedVideosCount === 0) 
                        return i18n.tr("Delete 1 photo");
                    else
                        return i18n.tr("Delete 1 video");
                } else {
                    if (organicEventView.selection.selectedVideosCount === 0)
                        return i18n.tr("Delete %1 photos").arg(organicEventView.selection.selectedPhotosCount);
                    else if (organicEventView.selection.selectedPhotosCount === 0)
                        return i18n.tr("Delete %1 videos").arg(organicEventView.selection.selectedVideosCount);
                    else {
                        if (organicEventView.selection.selectedVideosCount === 1 && organicEventView.selection.selectedPhotosCount !== 1)
                            return i18n.tr("Delete %1 photos and 1 video").arg(organicEventView.selection.selectedPhotosCount);
                        else if (organicEventView.selection.selectedPhotosCount === 1 && organicEventView.selection.selectedVideosCount !== 1)
                            return i18n.tr("Delete 1 photo and %1 videos").arg(organicEventView.selection.selectedVideosCount);
                        else if (organicEventView.selection.selectedVideosCount === 1 && organicEventView.selection.selectedPhotosCount === 1)
                            return i18n.tr("Delete 1 photo and 1 video");
                        else
                            return i18n.tr("Delete %1 photos and %2 videos").arg(organicEventView.selection.selectedPhotosCount)
                                                                            .arg(organicEventView.selection.selectedVideosCount);
                    }
                }
            }

            onDeleteClicked: {
                organicEventView.selection.model.destroySelectedMedia();
                organicEventView.leaveSelectionMode();
            }
        }
    }

    property Item overviewTools: PhotosToolbarActions {
        selection: organicEventView.selection
    }

    property int __pickerContentHeight: height - units.gu(20)
    property PopupAlbumPicker __albumPicker
    Connections {
        target: __albumPicker
        onAlbumPicked: {
            album.addSelectedMediaSources(selection.model.selectedMedias);
            organicEventView.leaveSelectionMode();
        }
    }

    property Item selectionTools: SelectionToolbarAction {
        selection: organicEventView.selection

        onCancelClicked: {
            organicEventView.leaveSelectionMode();
        }
        onAddClicked: {
            __albumPicker = PopupUtils.open(Qt.resolvedUrl("Components/PopupAlbumPicker.qml"),
                                            null,
                                            {contentHeight: organicEventView.__pickerContentHeight});
        }
        onDeleteClicked: {
            PopupUtils.open(deleteDialog, null);
        }

        onShareClicked: {
            overview.pushPage(sharePicker);
            sharePicker.visible = true;
        }
    }

    property bool selectionMode: selection.inSelectionMode
    tools: selectionMode ? selectionTools : overviewTools

    Component {
        id: contentItemComp
        ContentItem {}
    }

    Page {
        id: sharePicker
        visible: false
        title: i18n.tr("Share to")

        ContentPeerPicker {
            objectName: "sharePickerEvents"
            anchors.fill: parent
            showTitle: false

            contentType: organicEventView.selection.mediaType === MediaSource.Video ? ContentType.Videos : ContentType.Pictures
            handler: ContentHandler.Share

            onPeerSelected: {
                overview.popPage();
                sharePicker.visible = false;

                var curTransfer = peer.request();
                if (curTransfer.state === ContentTransfer.InProgress)
                {
                    var medias = organicEventView.selection.model.selectedMediasQML;
                    curTransfer.items = medias.filter(function(data) {
                        return data.hasOwnProperty('type'); // filter out event headers
                    }).map(function(data) {
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

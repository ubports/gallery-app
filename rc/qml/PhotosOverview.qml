/*
 * Copyright (C) 2012-2015 Canonical Ltd
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

import QtQuick 2.4
import Gallery 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Content 1.3
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

    function positionViewAtBeginning() {
        photosGrid.positionViewAtBeginning();
        //FIXME Initial content is hide bellow Page Header after a positionViewAtBeginning()
        photosGrid.contentY = units.gu(7) * -1;
    }

    function positionViewAtSelected(index) {
        photosGrid.positionViewAtIndex(index, GridView.Center);
    }

    property string pageTitle

    head.actions: inSelectionMode ? d.selectActions : d.overviewActions
    head.backAction: inSelectionMode ? d.selectBackAction : null

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

        property list<Action> overviewActions: [
            Action {
                objectName: "selectButton"
                text: i18n.tr("Select")
                iconName: "select"
                enabled: d.selection !== null
                onTriggered: d.selection.inSelectionMode = true;
            },
            Action {
                objectName: "cameraButton"
                text: i18n.tr("Camera")
                visible: !APP.desktopMode
                iconName: "camera-app-symbolic"
                onTriggered: Qt.openUrlExternally("appid://com.ubuntu.camera/camera/current-user-version")
            }
        ]

        property list<Action> selectActions: [
            Action {
                id: addButton
                objectName: "addButton"
 
                text: i18n.tr("Add")
                iconName: "add"
                enabled: d.selection.selectedCount > 0
                onTriggered: __albumPicker = PopupUtils.open(Qt.resolvedUrl("Components/PopupAlbumPicker.qml"),
                                                             null,
                                                             {contentHeight: photosOverview.__pickerContentHeight});
 
            },
            Action {
                objectName: "deleteButton"

                text: i18n.tr("Delete")
                iconName: "delete"
                enabled: d.selection.selectedCount > 0
                onTriggered: PopupUtils.open(deleteDialog, null);
            },
            Action {
                objectName: "shareButton"
                text: i18n.tr("Share")
                iconName: "share"
                enabled: d.selection.selectedMediaCount == 1
                onTriggered: {
                    overview.pushPage(sharePicker)
                    sharePicker.visible = true;
                }
            }
        ]

        property Action selectBackAction: Action {
            text: i18n.tr("Cancel")
            iconName: "back"
            onTriggered: photosOverview.leaveSelectionMode();
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

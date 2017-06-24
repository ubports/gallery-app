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
 * Authors
 * Emanuele Sorce <emanuele.sorce@hotmail.com>
 * 
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Gallery 1.0
import Ubuntu.Content 1.3
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
                if (organicEventView.selection.selectedVideosCount === 0) {
                    //No videos
                    return i18n.tr("Delete %1 photo",
                                   "Delete %1 photos",
                                   organicEventView.selection.selectedPhotosCount)
                                   .arg(organicEventView.selection.selectedPhotosCount);
                } else if (organicEventView.selection.selectedPhotosCount === 0) {
                    //No photos
                    return i18n.tr("Delete %1 video",
                                   "Delete %1 videos",
                                   organicEventView.selection.selectedVideosCount)
                                   .arg(organicEventView.selection.selectedVideosCount);
                } else {
                    //Mix of photos and videos
                    return i18n.tr("Delete %1 media file",
                                   "Delete %1 media files",
                                   organicEventView.selection.selectedMediaCount)
                                   .arg(organicEventView.selection.selectedMediaCount);
                }
            }

            onDeleteClicked: {
                organicEventView.selection.model.destroySelectedMedia();
                organicEventView.leaveSelectionMode();
            }
        }
    }

    property list<Action> overviewActions: [
        Action {
            objectName: "selectButton"
            text: i18n.tr("Select")
            iconName: "select"
            enabled: selection !== null
            onTriggered: selection.inSelectionMode = true;
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
            enabled: selection.selectedCount > 0
            onTriggered: __albumPicker = PopupUtils.open(Qt.resolvedUrl("Components/PopupAlbumPicker.qml"),
                                                         null,
                                                         {contentHeight: organicEventView.__pickerContentHeight});
 
        },
        Action {
            objectName: "deleteButton"

            text: i18n.tr("Delete")
            iconName: "delete"
            enabled: selection.selectedCount > 0
            onTriggered: PopupUtils.open(deleteDialog, null);
        },
        Action {
            objectName: "shareButton"
            text: i18n.tr("Share")
            iconName: "share"
            enabled: selection.selectedMediaCount > 0
            onTriggered: {
                if (selection.isMixed) {
                    PopupUtils.open(unableShareDialog, null);
                    return;
                }
                overview.pushPage(sharePicker);
                sharePicker.visible = true;
            }
        }
    ]

    property Action selectBackAction: Action {
        text: i18n.tr("Cancel")
        iconName: "back"
        onTriggered: organicEventView.leaveSelectionMode();
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

    property bool selectionMode: selection.inSelectionMode

    head.actions: selectionMode ? selectActions : overviewActions
    head.backAction: selectionMode ? selectBackAction : null

    Component {
        id: unableShareDialog
        UnableShareDialog {}
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

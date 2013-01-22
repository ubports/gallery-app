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
import Gallery 1.0
import "../../js/GalleryUtility.js" as GalleryUtility
import "../../../rc/Capetown"
import "../../../rc/Capetown/Widgets"
import "../Widgets"
import Ubuntu.Components 0.1 as Ubuntu // avoid name conflict until ChromeBar is deleted

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
OrganicView {
    id: organicEventView

    // Arbitrary extra amount of padding so that as you scroll the tray, the
    // photos are already loaded by the time they're on screen.
    property real trayLoadAreaPadding: units.gu(1)

    AlbumCollectionModel {
        id: albumCollectionModel
    }

    selection: SelectionState {
        // avoid entering selection mode by long-pressing on a photo:
        allowSelectionModeChange: false
    }

    model: EventCollectionModel {
    }

    delegate: Flickable {
        id: tray

        width: organicEventView.width
        height: photosList.height

        contentWidth: photosList.width
        contentHeight: photosList.height
        flickableDirection: Flickable.HorizontalFlick
        maximumFlickVelocity: units.gu(300)
        flickDeceleration: maximumFlickVelocity / 3

        onMovementStarted: trayLoadAreaPadding = units.gu(20)

        OrganicMediaList {
            id: photosList
            objectName: "eventViewPhoto" + index

            loadAreaLeft: tray.contentX - trayLoadAreaPadding
            // size + one big thumbnail
            loadAreaWidth: tray.width + 2 * trayLoadAreaPadding + bigSize

            animationDuration: organicEventView.animationDuration
            animationEasingType: organicEventView.animationEasingType

            event: model.event
            selection: organicEventView.selection

            onPressed: {
                var rect = GalleryUtility.translateRect(thumbnailRect, photosList,
                                                        organicEventView);
                organicEventView.mediaSourcePressed(mediaSource, rect);
            }
        }
    }

    property Item overviewTools: Row {
        ChromeButton {
            text: "Select"
            icon: Qt.resolvedUrl("../../img/select.png")
            onClicked: {
                // Set inSelectionMode instead of using tryEnterSelectionMode
                // because allowSelectionModeChange is false.
                selection.inSelectionMode = true;
            }
        }
        ChromeButton {
            text: "Import"
            icon: Qt.resolvedUrl("../../img/import-image.png")
            enabled: false
        }
        ChromeButton {
            text: "Camera"
            icon: Qt.resolvedUrl("../../img/camera.png")
            onClicked: {
                if (appManager.status == Loader.Ready) appManager.item.switchToCameraApplication();
                else console.log("Switching applications is not supported on this platform.");
            }
        }
        Loader {
            id: appManager
            source: "../../../rc/Capetown/Widgets/UbuntuApplicationWrapper.qml"
        }
    }

    DeletePopover {
        objectName: "eventsViewDeletePopover"
        visible: false
        id: deletePopover
        onDeleteClicked: {
            organicEventView.selection.model.destroySelectedMedia();
            deletePopover.hide();
            selectionTools.leaveSelectionMode();
        }
    }

    property Item selectionTools: Row {
        function leaveSelectionMode() {
            // Set inSelectionMode instead of using leaveSelectionMode()
            // because allowSelectionModeChange is false
            selection.unselectAll();
            selection.inSelectionMode = false;
        }
        ChromeButton { // TODO: This special button should go on the left
            text: "Cancel"
            icon: Qt.resolvedUrl("../../img/cancel.png")
            onClicked: selectionTools.leaveSelectionMode()
        }
        ChromeButton {
            text: "Add"
            icon: Qt.resolvedUrl("../../img/add.png")
            enabled: selection.selectedCount > 0
            onClicked: {
                var album = albumCollectionModel.createOrphan();
                album.addSelectedMediaSources(selection.model);
                albumCollectionModel.addOrphan(album);

                // We can't use leaveSelectionMode() here, due to the fact that
                // we're skirting around the proper use of the selection object.
                selection.unselectAll();
                selection.inSelectionMode = false;
            }

        }
        ChromeButton {
            id: deleteButton
            text: "Delete"
            icon: Qt.resolvedUrl("../../img/delete.png")
            enabled: selection.selectedCount > 0
            onClicked: {
                deletePopover.caller = deleteButton;
                deletePopover.show();
            }
        }
        ChromeButton {
            text: "Share"
            icon: Qt.resolvedUrl("../../img/share.png")
            enabled: false
        }
    }

    property bool selectionMode: selection.inSelectionMode

    property Item tools: selectionMode ? selectionTools : overviewTools
    onToolsChanged: print("tools changed to "+tools)

    Ubuntu.Toolbar {
        id: toolbar
        page: organicEventView
        tools: organicEventView.tools
    }
}

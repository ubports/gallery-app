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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import "../../js/GalleryUtility.js" as GalleryUtility
import "../../../rc/Capetown"
import "../../../rc/qml/Widgets"

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
OrganicView {
    id: organicEventView

    // Arbitrary extra amount of padding so that as you scroll the tray, the
    // photos are already loaded by the time they're on screen.
    property real trayLoadAreaPadding: units.gu(20)

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

        OrganicMediaList {
            id: photosList

            loadAreaLeft: tray.contentX - trayLoadAreaPadding
            loadAreaRight: tray.contentX + tray.width + trayLoadAreaPadding

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

    ChromeBar {
        id: chromeBar
        //        z: 100
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        property bool selectionMode: selection.inSelectionMode
        showBackButton: selectionMode
        backButtonIcon: Qt.resolvedUrl("../../img/cancel.png")
        backButtonText: "Cancel"
        onBackButtonClicked: leaveSelectionMode();

        function leaveSelectionMode() {
            // Set inSelectionMode instead of using leaveSelectionMode()
            // because allowSelectionModeChange is false
            selection.unselectAll();
            selection.inSelectionMode = false;
        }

        buttonsModel: selectionMode ? selectionModel : overviewModel

        property ListModel selectionModel: ListModel {
            ListElement {
                label: "Add"
                name: "disabled"
                icon: "../img/add.png"
            }
            ListElement {
                label: "Delete"
                name: "delete"
                icon: "../img/delete.png"
            }
            ListElement {
                label: "Share"
                name: "disabled"
                icon: "../img/share.png"
            }
        }

        property ListModel overviewModel: ListModel {
            ListElement {
                label: "Select"
                name: "select"
                icon: "../img/select.png"
            }
            ListElement {
                label: "Import"
                name: "disabled"
                icon: "../img/import-image.png"
            }
            ListElement {
                label: "Camera"
                name: "disabled"
                icon: "../img/camera.png"
            }
        }
        showChromeBar: true

        onButtonClicked: {
            switch (buttonName) {
            case "select": {
                // Set inSelectionMode instead of using tryEnterSelectionMode
                // because allowSelectionModeChange is false.
                selection.inSelectionMode = true;
                break;
            }
            case "delete": {
                deletePopover.caller = button;
                deletePopover.show();
                break;
            }
            }
        }

        Popover {
            visible: false
            id: deletePopover

            Column {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                ListItem.SingleControl {
                    control: Button {
                        color: "red"
                        text: "Delete selected items"
                        anchors.fill: parent
                        onClicked: {
                            organicEventView.selection.model.destroySelectedMedia();
                            deletePopover.hide();
                            chromeBar.leaveSelectionMode();
                        }
                    }
                }
            }
        }
    }
}

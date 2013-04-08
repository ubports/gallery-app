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
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
OrganicView {
    id: organicEventView

    // Arbitrary extra amount of padding so that as you scroll the tray, the
    // photos are already loaded by the time they're on screen.
    property real trayLoadAreaPadding: units.gu(1)

    /// True if in the selection mode
    property alias inSelectionMode: select.inSelectionMode

    /// Quit selection mode, and unselect all photos
    function leaveSelectionMode() {
        selection.unselectAll()
        selection.inSelectionMode = false
    }

    AlbumCollectionModel {
        id: albumCollectionModel
    }

    selection: SelectionState {
        id: select
        // avoid entering selection mode by long-pressing on a photo:
        allowSelectionModeChange: false
    }

    model: EventCollectionModel {
    }

    delegate: OrganicMediaList {
        id: photosList
        objectName: "organicEventItem" + index

        width: organicEventView.width

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

    Component {
        id: deleteDialog
        DeleteDialog {
            //FIXME this count > 1 thing needs to be properly replaced by translation wrappers allowing for multiple plural forms
            title: organicEventView.selection.selectedCount > 1 ? "Delete photos" : "Delete a photo"

            onDeleteClicked: {
                organicEventView.selection.model.destroySelectedMedia();
                organicEventView.leaveSelectionMode();
            }
        }
    }

    PopupAlbumPicker {
        id: albumPicker
        contentHeight: parent.height - units.gu(20)

        onAlbumPicked: {
            album.addSelectedMediaSources(selection.model);
            organicEventView.leaveSelectionMode()
        }
    }

    property ActionList overviewTools: PhotosToolbarActions {
        selection: organicEventView.selection
        onStartCamera: appManager.switchToCameraApplication();
    }

    UbuntuApplicationCaller {
        id: appManager
    }

    property ActionList selectionTools: SelectionToolbarAction {
        selection: organicEventView.selection

        onCancelClicked: {
            organicEventView.leaveSelectionMode();
        }
        onAddClicked: {
            albumPicker.caller = caller
            albumPicker.show()
        }
        onDeleteClicked: {
            PopupUtils.open(deleteDialog, null);
        }
    }

    property bool selectionMode: selection.inSelectionMode
    tools: selectionMode ? selectionTools : overviewTools
}

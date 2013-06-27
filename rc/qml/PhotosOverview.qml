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

    /// True if in the selection mode
    property alias inSelectionMode: d.inSelectionMode

    /// Sets the model to the grid if not done yet (lazy initialization)
    function initModel() {
        if (!photosGrid.model)
            photosGrid.model = d.model
    }

    /// Quit selection mode, and unselect all photos
    function leaveSelectionMode() {
        d.selection.unselectAll()
        d.selection.inSelectionMode = false
    }

    tools: inSelectionMode ? d.selectionTools : d.overviewTools

    Image {
        anchors.fill: parent

        source: "../img/background-paper.png"
        fillMode: Image.Tile
    }

    MediaGrid {
        id: photosGrid

        anchors.fill: parent
        selection: d.selection
    }

    Component {
        id: deleteDialog
        DeleteDialog {
            //FIXME this count > 1 thing needs to be properly replaced by translation wrappers allowing for multiple plural forms
            title: d.selection.selectedCount > 1 ? i18n.tr("Delete photos") :
                                                   i18n.tr("Delete a photo")

            onDeleteClicked: {
                d.selection.model.destroySelectedMedia();
                photosOverview.leaveSelectionMode();
            }
        }
    }

    PopupAlbumPicker {
        id: albumPicker
        objectName: "photosPopupAlbumPicker"

        contentHeight: parent.height - units.gu(20)
        onAlbumPicked: {
            album.addSelectedMediaSources(d.selection.model)
            photosOverview.leaveSelectionMode()
        }
    }

    Item {
        id: d

        property MediaCollectionModel model: MediaCollectionModel {
            monitored: true
        }

        property bool inSelectionMode: selection.inSelectionMode
        property SelectionState selection: SelectionState {
            model: d.model
        }

        property Item overviewTools: PhotosToolbarActions {
            selection: d.selection
            onStartCamera: appManager.switchToCameraApplication();
        }

        UbuntuApplicationCaller {
            id: appManager
        }

        property Item selectionTools: SelectionToolbarAction {
            selection: d.selection

            onCancelClicked: {
                photosOverview.leaveSelectionMode();
            }
            onAddClicked: {
                albumPicker.caller = caller;
                albumPicker.show();
            }
            onDeleteClicked: {
                PopupUtils.open(deleteDialog, null);
            }
        }
    }
}

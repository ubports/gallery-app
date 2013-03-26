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
import "Utility"
import "Widgets"
import "../Capetown/Widgets"
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

    GridView {
        id: photosGrid

        anchors.fill: parent
        anchors.leftMargin: units.gu(1)
        anchors.rightMargin: units.gu(1)

        /// Size of the thumbnails
        property real thumbnailSize: units.gu(12)
        /// Minimum space between the tumbnails
        property real minimumSpace: units.gu(0.6)
        /// Stores the spacing between 2 images in pixel
        property real spacing: calculateSpacing(width)

        /*!
        Calculates the spacing that should be used, to fit the fotos horizontally nicely
        */
        function calculateSpacing(viewWidth) {
            var itemSize = thumbnailSize + minimumSpace
            var itemCount = Math.floor(viewWidth / itemSize)
            var spareSpace = viewWidth - itemCount * itemSize
            return (minimumSpace + spareSpace / itemCount)
        }

        cellWidth: thumbnailSize + spacing
        cellHeight: thumbnailSize + spacing

        maximumFlickVelocity: units.gu(800)
        flickDeceleration: maximumFlickVelocity * 0.5

        // Use this rather than anchors.topMargin to prevent delegates from being
        // unloade while scrolling out of view but still partially visible
        header: Item {
            width: parent.width
            height: units.gu(1)
        }

        delegate: Item {
            objectName: "allPotosGridPhoto"
            width: photosGrid.cellWidth
            height: photosGrid.cellHeight

            UbuntuShape {
                id: roundedThumbnail

                anchors.centerIn: parent

                width: photosGrid.thumbnailSize
                height: photosGrid.thumbnailSize

                radius: "medium"

                image: Image {
                    source: mediaSource.galleryThumbnailPath
                    asynchronous: true
                }

                OrganicItemInteraction {
                    objectName: "photosViewPhoto"
                    selectionItem: model.mediaSource
                    selection: d.selection

                    onPressed: {
                        var rect = GalleryUtility.getRectRelativeTo(roundedThumbnail, photosOverview);
                        photosOverview.mediaSourcePressed(mediaSource, rect);
                    }
                }
            }
        }

        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: Gallery.FAST_DURATION
                easing.type: Easing.InQuint
            }
        }
    }

    Component {
        id: deleteDialog
        DeleteDialog {
            //FIXME this count > 1 thing needs to be properly replaced by translation wrappers allowing for multiple plural forms
            title: d.selection.selectedCount > 1 ? "Delete photos" : "Delete a photo"

            onDeleteClicked: {
                d.selection.model.destroySelectedMedia();
                photosOverview.leaveSelectionMode();
            }
        }
    }

    PopupAlbumPicker {
        id: albumPicker
        objectName: "photosPopupAlbumPicker"

        visible: false
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

        property ActionList overviewTools: PhotosToolbarActions {
            selection: d.selection
            onStartCamera: appManager.switchToCameraApplication();
        }

        UbuntuApplicationCaller {
            id: appManager
        }

        property ActionList selectionTools: SelectionToolbarAction {
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

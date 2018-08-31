/*
 * Copyright (C) 2013-2015 Canonical Ltd
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Thumbnailer 0.1
import Gallery 1.0
import "../OrganicView"
import "../Utility"
import "../../js/Gallery.js" as Gallery
import "../../js/GalleryUtility.js" as GalleryUtility

/*!
Shows a grid with media from a given model. The medias can be selected if a
proper SelectionSate is given
 */
GridView {

    id: photosGrid

    /// Selection for the grid
    property SelectionState selection: null

    /// Size of the thumbnails
    property real thumbnailSize: photosGrid.gridSize
    /// Size of the requested thumbnails
    property real thumbnailSourceSize: units.gu(19)
    /// Minimum space between the tumbnails
    property real minimumSpace: units.gu(0.6)
    /// Stores the spacing between 2 images in pixel
    property real spacing: __calculateSpacing(width)

    /*!
    Calculates the spacing that should be used, to fit the fotos horizontally nicely
    */
    function __calculateSpacing(viewWidth) {
        var itemSize = thumbnailSize + minimumSpace
        var itemCount = Math.floor(viewWidth / itemSize)
        var spareSpace = viewWidth - itemCount * itemSize
        return (minimumSpace + spareSpace / itemCount)
    }

    anchors.leftMargin: units.gu(1)
    anchors.rightMargin: units.gu(1)

    cellWidth: thumbnailSize + spacing
    cellHeight: thumbnailSize + spacing

    maximumFlickVelocity: units.gu(800)
    flickDeceleration: units.gu(400)

    // Use this rather than anchors.topMargin to prevent delegates from being
    // unloaded while scrolling out of view but still partially visible
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
            property bool isLoading: source.status === Image.Loading

            backgroundColor: "black"
            sourceFillMode: UbuntuShape.PreserveAspectCrop
            source: Image {
                id: thumbImage
                source: "image://thumbnailer/" + mediaSource.path + "?at=" + mediaSource.lastModified
                asynchronous: true
                fillMode: Image.PreserveAspectCrop
                sourceSize {
                    width: thumbnailSourceSize
                    height: thumbnailSourceSize
                }
            }

            Icon {
                anchors.centerIn: parent
                width: units.gu(6)
                height: width
                visible: thumbImage.status == Image.Error
                name: "stock_image"
                color: "white"
                opacity: 0.8
            }

            Icon {
				// Display a play icon if the thumbnail is from a video
				anchors.centerIn: parent
				width: units.gu(5)
				height: units.gu(5)
				name: "media-playback-start"
				color: "white"
				visible: mediaSource.type === MediaSource.Video && thumbImage.status == Image.Ready
			}

            OrganicItemInteraction {
                objectName: "photosViewPhoto"
                selectionItem: model.mediaSource
                selection: photosGrid.selection ? photosGrid.selection : null

                onPressed: {
                    var rect = GalleryUtility.getRectRelativeTo(roundedThumbnail, photosOverview);
                    photosOverview.mediaSourcePressed(mediaSource, rect);
                }

                onSelected: photosGrid.positionViewAtIndex(index, GridView.Contain);
            }

            Component {
                id: component_loadIndicator
                ActivityIndicator {
                    id: loadIndicator
                    running: true
                }
            }
            Loader {
                id: loader_loadIndicator
                anchors.centerIn: roundedThumbnail
                sourceComponent: roundedThumbnail.isLoading ? component_loadIndicator : undefined
                asynchronous: true
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

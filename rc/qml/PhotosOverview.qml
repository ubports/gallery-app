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
import "Utility"
import "../js/GalleryUtility.js" as GalleryUtility

Item {
  id: photosOverview

  signal mediaSourcePressed(var mediaSource, var thumbnailRect)

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

    cellWidth: units.gu(12) + units.gu(0.75)
    cellHeight: units.gu(12) + units.gu(0.75)

    maximumFlickVelocity: units.gu(800)
    flickDeceleration: maximumFlickVelocity * 0.5

    model: MediaCollectionModel {
      monitored: true
    }

    // Use this rather than anchors.topMargin to prevent delegates from being
    // unloade while scrolling out of view but still partially visible
    header: Item {
      width: parent.width
      height: units.gu(1)
    }
  
    delegate: Item {
      width: photosGrid.cellWidth
      height: photosGrid.cellHeight

      RoundCornerShape {
        id: roundedThumbnail

        anchors.centerIn: parent

        width: units.gu(12)
        height: units.gu(12)

        radius: "medium"

        image: Image {
          source: mediaSource.thumbnailPath
        }

        MouseArea {
          anchors.fill: parent

          onClicked: {
            mediaSourcePressed(mediaSource, GalleryUtility.getRectRelativeTo(
              roundedThumbnail, photosOverview));
          }
        }
      }
    }
  }
}

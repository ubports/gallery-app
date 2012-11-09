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

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
Item {
  id: organicPhotosView

  property alias model: organicList.model

  Image {
    anchors.fill: parent

    source: "../../img/background-paper.png"
    fillMode: Image.Tile
  }

  ListView {
    id: organicList

    anchors.fill: parent
    clip: true

    model: EventCollectionModel {
    }

    delegate: Flickable {
      id: flickableTray

      width: organicPhotosView.width
      height: photosList.height
      clip: true

      contentWidth: photosList.width
      contentHeight: photosList.height
      flickableDirection: Flickable.HorizontalFlick

      OrganicPhotosList {
        id: photosList

        event: model.event
      }
    }
  }
}

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
import "../Components"

// An "organic" list of photos for a particular event.  Used as the "tray"
// contents for each event in the OrganicPhotosView.
Item {
  id: organicPhotosList

  property var event

  width: childrenRect.width // TODO: + margins
  height: childrenRect.height // TODO: + margins

  // TODO: make this organic looking.
  Row {
    EventCard {
      event: organicPhotosList.event
    }

    Repeater {
      id: repeater

      model: MediaCollectionModel {
        forCollection: organicPhotosList.event
        monitored: true
      }

      MattedPhotoPreview {
        mediaSource: model.mediaSource
        ownerName: "OrganicTrayView"
      }
    }
  }
}

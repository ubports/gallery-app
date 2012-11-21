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
import "../../js/GalleryUtility.js" as GalleryUtility

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
OrganicView {
  id: organicEventView

  selection: SelectionState {
  }

  model: EventCollectionModel {
  }

  delegate: Flickable {
    id: tray

    width: organicEventView.width
    height: photosList.height
    clip: true

    contentWidth: photosList.width
    contentHeight: photosList.height
    flickableDirection: Flickable.HorizontalFlick

    OrganicMediaList {
      id: photosList

      // Arbitrary extra amount of padding so that as you scroll, the photos
      // are already loaded by the time they're on screen.
      property real loadPadding: gu(10)

      loadAreaLeft: tray.contentX - loadPadding
      loadAreaRight: tray.contentX + tray.width + loadPadding

      event: model.event
      selection: organicEventView.selection

      onPressed: {
        var rect = GalleryUtility.translateRect(thumbnailRect, photosList,
                                                organicEventView);
        organicEventView.mediaSourcePressed(mediaSource, rect);
      }
    }
  }
}

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
import "../../js/GalleryUtility.js" as GalleryUtility

// An "organic" list of photos for a particular event.  Used as the "tray"
// contents for each event in the OrganicPhotosView.
Item {
  id: organicPhotosList

  signal pressed(var mediaSource, var thumbnailRect)

  property var event
  property alias mediaSources: photosRepeater.model
  property OrganicSelectionState selection

  // internal
  // This assumes an internal margin of gu(3), and a particular pattern of
  // photos and event cards with sizes of gu(27) and gu(18) depending on
  // placement.
  property int photosPerPattern: 6
  property var photoX: [gu(0), gu(0), gu(21), gu(30), gu(51), gu(42)]
  property var photoY: [gu(0), gu(30), gu(30), gu(0), gu(0), gu(21)]
  property var photoLength: [gu(27), gu(18), gu(18), gu(18), gu(18), gu(27)]
  property real patternWidth: gu(72) // one big, two small, and margins
  property real photosLeftMargin: gu(24) // one event card + 2 margins
  property real photosTopMargin: gu(1.5) // half a margin

  width: childrenRect.width + gu(3) // full margin at the end
  height: childrenRect.height + gu(1.5) // half a margin on the bottom

  EventCard {
    id: eventComponent

    x: gu(3)
    y: gu(1.5)
    width: gu(18)
    height: gu(18)

    event: organicPhotosList.event
  }

  // This should theoretically go inside the event card, but EventCard does
  // weird scaling to draw correctly which messes this up if it's inside it.
  OrganicItemInteraction {
    anchors.fill: eventComponent
    selectionItem: event
    selection: organicPhotosList.selection
  }

  // TODO: for performance, we may want to use something else here.  Repeaters
  // load all their delegates at once, which may cause slow scrolling in the
  // OrganicPhotosView.  Alternately, we may be able to pass in the visible
  // area from the parent Flickable and only set photos visible (and thus
  // trigger a load from disk) when they're in the visible area.
  Repeater {
    id: photosRepeater

    model: MediaCollectionModel {
      forCollection: organicPhotosList.event
      monitored: true
    }

    // TODO: rounded corners.
    GalleryPhotoComponent {
      id: photoComponent

      property int patternPhoto: index % photosPerPattern
      property int patternNumber: Math.floor(index / photosPerPattern)

      x: photosLeftMargin + photoX[patternPhoto] + patternWidth * patternNumber
      y: photosTopMargin + photoY[patternPhoto]
      width: photoLength[patternPhoto]
      height: photoLength[patternPhoto]

      mediaSource: model.mediaSource
      ownerName: "OrganicTrayView"
      isCropped: true
      isPreview: true

      OrganicItemInteraction {
        selectionItem: photoComponent.mediaSource
        selection: organicPhotosList.selection

        onPressed: {
          var rect = GalleryUtility.getRectRelativeTo(photoComponent,
                                                      organicPhotosList);
          organicPhotosList.pressed(photoComponent.mediaSource, rect);
        }
      }
    }
  }
}

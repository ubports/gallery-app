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

// An "organic" list of photos.  Used as the "tray" contents for each event in
// the OrganicEventView, and the layout of the OrganicAlbumView.
Item {
  id: organicMediaList

  signal pressed(var mediaSource, var thumbnailRect)

  property var event
  property alias mediaModel: photosRepeater.model
  property SelectionState selection

  // The left and right edges of the region in which to load photos; any
  // outside this region are created as delegates, but the photo isn't loaded.
  property real loadAreaLeft: 0
  property real loadAreaRight: width

  // readonly
  property int mediaPerPattern: 6
  property real patternWidth: gu(72) // one big, two small, and margins
  property real margin: gu(3)

  // internal
  // This assumes an internal margin of gu(3), and a particular pattern of
  // photos and event cards with sizes of gu(27) and gu(18) depending on
  // placement.  I didn't want to actually put the math in the QML because it's
  // complicated and I didn't want to slow down the binding.  It just means
  // this will be a pain to update if they change the design.
  property var photoX: [gu(0), gu(0), gu(21), gu(30), gu(51), gu(42)]
  property var photoY: [gu(0), gu(30), gu(30), gu(0), gu(0), gu(21)]
  property var photoLength: [gu(27), gu(18), gu(18), gu(18), gu(18), gu(27)]
  property real photosLeftMargin: (event ? gu(24) : margin) // optional event card + margins
  property real photosTopMargin: margin / 2

  width: childrenRect.width + margin
  height: childrenRect.height + margin / 2

  EventCard {
    x: margin
    y: photosTopMargin
    width: gu(18)
    height: gu(18)

    visible: Boolean(event)

    event: organicMediaList.event

    OrganicItemInteraction {
      selectionItem: event
      selection: organicMediaList.selection
    }
  }

  // TODO: for performance, we may want to use something else here.  Repeaters
  // load all their delegates at once, which may cause slow scrolling in the
  // OrganicEventView.  Alternately, we may be able to pass in the visible
  // area from the parent Flickable and only set photos visible (and thus
  // trigger a load from disk) when they're in the visible area.
  Repeater {
    id: photosRepeater

    model: MediaCollectionModel {
      forCollection: organicMediaList.event
      monitored: true
    }

    // TODO: rounded corners.
    GalleryPhotoComponent {
      id: photoComponent

      property int patternPhoto: index % mediaPerPattern
      property int patternNumber: Math.floor(index / mediaPerPattern)
      property var modelMediaSource: model.mediaSource

      x: photosLeftMargin + photoX[patternPhoto] + patternWidth * patternNumber
      y: photosTopMargin + photoY[patternPhoto]
      width: photoLength[patternPhoto]
      height: photoLength[patternPhoto]

      mediaSource: (x <= loadAreaRight && x + width >= loadAreaLeft
                    ? modelMediaSource : null)
      ownerName: "OrganicMediaList"
      isCropped: true
      isPreview: true

      OrganicItemInteraction {
        selectionItem: photoComponent.modelMediaSource
        selection: organicMediaList.selection

        onPressed: {
          var rect = GalleryUtility.getRectRelativeTo(photoComponent,
                                                      organicMediaList);
          organicMediaList.pressed(photoComponent.modelMediaSource, rect);
        }
      }
    }
  }
}

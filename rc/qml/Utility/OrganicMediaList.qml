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
import Ubuntu.Components 0.1
import "../Components"
import "../../js/Gallery.js" as Gallery
import "../../js/GalleryUtility.js" as GalleryUtility

// An "organic" list of photos.  Used as the "tray" contents for each event in
// the OrganicEventView, and the layout of the OrganicAlbumView.
Item {
  id: organicMediaList
  objectName: "organicMediaList"

  /*!
  */
  signal pressed(var mediaSource, var thumbnailRect)

  /*!
  */
  property var event
  /*!
  */
  property alias mediaModel: photosRepeater.model
  /*!
  */
  property SelectionState selection

  // The left and right edges of the region in which to load photos; any
  // outside this region are created as delegates, but the photo isn't loaded.
  property real loadAreaLeft: 0
  /*!
  */
  property real loadAreaWidth: width

  /*!
  */
  property int animationDuration: Gallery.FAST_DURATION
  /*!
  */
  property int animationEasingType: Easing.InQuint

  // readonly
  /*!
  */
  property int mediaPerPattern: 6
  /*!
  */
  property var bigSize: units.gu(19)
  /*!
  */
  property var smallSize: units.gu(12)
  /*!
  */
  property real margin: units.gu(2)
  /*!
  */
  property real patternWidth: bigSize + smallSize * 2 + margin * 3
  /*!
  */
  property var patternLeftWidth: [0, bigSize, bigSize, smallSize * 2 + margin,
      bigSize + margin + smallSize, bigSize + 2 * margin + 2 * smallSize ]

  // internal
  /*!
  */
  property var photoX: [0, 0, smallSize + margin, bigSize + margin,
      bigSize + smallSize + margin * 2, smallSize * 2 + margin * 2]
  /*!
  */
  property var photoY: [0, bigSize + margin, bigSize + margin, 0, 0,
      smallSize + margin]
  /*!
  */
  property var photoSize: [bigSize, smallSize, smallSize, smallSize, smallSize,
      bigSize]
  /*!
  */
  property real photosLeftMargin: margin + (event ? smallSize + margin : 0)
  /*!
  */
  property real photosTopMargin: margin / 2

  width: photosLeftMargin +
         Math.floor(mediaModel.count / mediaPerPattern) * patternWidth +
         patternLeftWidth[mediaModel.count % mediaPerPattern] +
         margin
  height: (mediaModel.count > 1) ?
              (bigSize + smallSize + photosTopMargin + margin + margin/2) :
              (bigSize + photosTopMargin + margin / 2)

  EventCard {
    x: margin
    y: photosTopMargin
    width: smallSize
    height: smallSize

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

    Item {
      id: tItem
      property int patternPhoto: index % mediaPerPattern
      property int patternNumber: Math.floor(index / mediaPerPattern)
      property bool isInLoadArea: xw <= loadAreaLeft && x >= loadAreaLeft

      x: photosLeftMargin + photoX[patternPhoto] + patternWidth * patternNumber
      // transform th right end to the left, for easier comparison for isInLoadArea
      property int xw: x - loadAreaWidth
      visible: isInLoadArea
      y: photosTopMargin + photoY[patternPhoto]
      width: photoSize[patternPhoto]
      height: photoSize[patternPhoto]

      ActivityIndicator {
          id: loadIndicator
          anchors.centerIn: parent
          visible: loader_thumbnail && loader_thumbnail.item && !loader_thumbnail.item.visible
          running: visible
      }

      Component {
        id: component_thumbnail
        UbuntuShape {
          id: thumbnail
          visible: false

          radius: "medium"

          image: Image {
            source: model.mediaSource.galleryThumbnailPath
            asynchronous: true
            onStatusChanged: {
                if (status===Image.Ready)
                    thumbnail.visible = true
            }
          }

          OrganicItemInteraction {
            objectName: "eventsViewPhoto"
            selectionItem: model.mediaSource
            selection: organicMediaList.selection

            onPressed: {
              var rect = GalleryUtility.getRectRelativeTo(thumbnail,
                                                          organicMediaList);
              organicMediaList.pressed(selectionItem, rect);
            }
          }

          // TODO: fade in photos being added, fade out ones being deleted?  This
          // might entail using Repeater's onItemAdded/onItemRemoved signals and
          // manually keeping around a list of thumbnails to animate, as we can't
          // very well animate the thumbnails created as Repeater delegates since
          // they'll be destroyed before the animation would finish.

          // x is intentionally missing here -- see below.
          Behavior on y {
            NumberAnimation {
              duration: animationDuration
              easing.type: animationEasingType
            }
          }
          Behavior on width {
            NumberAnimation {
              duration: animationDuration
              easing.type: animationEasingType
            }
          }
          Behavior on height {
            NumberAnimation {
              duration: animationDuration
              easing.type: animationEasingType
            }
          }
        }
      }
      Loader {
        id: loader_thumbnail
        anchors.fill: parent
        sourceComponent: tItem.isInLoadArea ? component_thumbnail : undefined
        asynchronous: true
      }

      // This is alone down here because the containing component only
      // specifies the x of the Loader; the loaded component specifies the y,
      // width, and height so they're above.
      Behavior on x {
        NumberAnimation {
          duration: animationDuration
          easing.type: animationEasingType
        }
      }
    }
  }
}

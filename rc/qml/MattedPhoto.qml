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

import QtQuick 1.1
import Gallery 1.0

// A PhotoComponent with a backing "mat and shadow" image and an internal
// overlay shadow image.
Item {
  id: mattedPhoto

  property alias ownerName: photo.ownerName
  property alias mediaSource: photo.mediaSource
  property alias isPreview: photo.isPreview

  property alias backingImage: backing.source
  property alias insideShadowImage: insideShadow.source
  property alias overlayImage: overlay.source

  property real mattedPhotoWidth
  property real mattedPhotoHeight
  // Where the transparent shadow ends in the backing image.
  property real backingStartX
  property real backingStartY

  // readonly
  property alias photoWidth: photo.width
  property alias photoHeight: photo.height

  Image {
    id: backing

    x: -backingStartX
    y: -backingStartY

    Item {
      // An area the size of the backing image (the non-transparent parts).
      x: backingStartX
      y: backingStartY
      width: mattedPhotoWidth
      height: mattedPhotoHeight

      GalleryPhotoComponent {
        id: photo

        width: insideShadow.width
        height: insideShadow.height
        anchors.centerIn: parent

        visible: (mediaSource) ? true : false

        isCropped: true
      }

      Image {
        id: insideShadow

        anchors.centerIn: parent
      }
      
      Image {
        id: overlay
        
        anchors.right: parent.right
        anchors.bottom: parent.bottom
      }
    }
  }
}

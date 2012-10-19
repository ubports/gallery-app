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
import "../../js/Gallery.js" as Gallery

// A PhotoComponent with a backing "mat and shadow" image and an internal
// overlay shadow image.  Unlike the CompositeMattedPhoto, this is always
// displayed at a constant size.
Item {
  id: mattedPhotoPreview

  property alias ownerName: photo.ownerName
  property alias mediaSource: photo.mediaSource
  property bool isSelected
  
  // Use this to make this element show an arbitrary image.
  property alias substituteSource: substitutePhoto.source

  // internal
  property real mattedPhotoWidth: gu(24)
  property real mattedPhotoHeight: gu(18)
  // Where the transparent shadow ends in the backing image.
  property real backingStartX: 2
  property real backingStartY: 2

  // readonly
  property alias photoWidth: photo.width
  property alias photoHeight: photo.height

  width: getDeviceSpecific("photoThumbnailWidth")
  height: getDeviceSpecific("photoThumbnailHeight")

  // TODO: we may want different graphical assets here instead of just scaling
  // down the tablet-sized ones.
  transform: Scale {
    xScale: width / mattedPhotoWidth
    yScale: height / mattedPhotoHeight
  }

  Image {
    id: backing

    x: -backingStartX
    y: -backingStartY

    source: isSelected ? "img/photo-preview-backing-selected.png"
                       : "img/photo-preview-backing.png"

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
        isPreview: true
      }
      
      Image {
        id: substitutePhoto
        
        width: insideShadow.width
        height: insideShadow.height
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        
        visible: source != null
      }
      
      Image {
        id: insideShadow

        anchors.centerIn: parent

        source: "img/photo-preview-inside-shadow.png"
      }

      Image {
        id: overlay

        anchors.right: parent.right
        anchors.bottom: parent.bottom

        source: isSelected ? "img/photo-preview-selected-overlay.png" : ""
      }
    }
  }
}

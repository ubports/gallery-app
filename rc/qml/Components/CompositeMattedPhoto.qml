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

// A PhotoComponent with a backing "mat and shadow" image and an internal
// overlay shadow image, all made of composite images (i.e. 4 corners, and 4
// repeated edges).
Item {
  id: compositeMattedPhoto

  property alias ownerName: photo.ownerName
  property alias mediaSource: photo.mediaSource
  property alias isPreview: photo.isPreview

  property alias backingBorderImage: backing.source
  property alias insideShadowBorderImage: insideShadow.source

  // Where the transparent shadow ends in the backing image.
  property real backingShadowPixelsLeft
  property real backingShadowPixelsRight
  property real backingShadowPixelsTop
  property real backingShadowPixelsBottom

  // How wide of a frame to use inside the backing image.
  property real backingFramePixelsLeft
  property real backingFramePixelsRight
  property real backingFramePixelsTop
  property real backingFramePixelsBottom

  // readonly
  property alias photoWidth: photo.width
  property alias photoHeight: photo.height

  BorderImage {
    id: backing

    x: -backingShadowPixelsLeft
    y: -backingShadowPixelsTop
    width: parent.width + backingShadowPixelsLeft + backingShadowPixelsRight
    height: parent.height + backingShadowPixelsTop + backingShadowPixelsBottom

    Item {
      // An area inside the "frame" part of the backing image.
      x: backingShadowPixelsLeft + backingFramePixelsLeft
      y: backingShadowPixelsTop + backingFramePixelsTop
      width: compositeMattedPhoto.width - backingFramePixelsLeft -
             backingFramePixelsRight
      height: compositeMattedPhoto.height - backingFramePixelsTop -
              backingFramePixelsBottom

      GalleryPhotoComponent {
        id: photo

        anchors.fill: parent

        visible: (mediaSource) ? true : false

        isCropped: true
      }

      BorderImage {
        id: insideShadow

        anchors.fill: parent
      }
    }
  }
}

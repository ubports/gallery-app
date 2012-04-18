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

// A preview PhotoComponent with an overlay mat image.  Could be expanded to
// handle more than just previews with a little work and a better image.
Item {
  id: mattedPhotoPreview

  property alias ownerName: preview.ownerName
  property alias mediaSource: preview.mediaSource

  // readonly
  property alias photoWidth: preview.width
  property alias photoHeight: preview.height

  // internal
  property real previewWidth: gu(24)
  property real previewHeight: gu(18)
  // Where the transparent shadow ends in the backing image.
  property real backingStartX: 2
  property real backingStartY: 2

  Image {
    x: -backingStartX
    y: -backingStartY

    source: "../img/photo-preview-backing.png"
    cache: true

    Item {
      // An area the size of the backing image (the non-transparent parts).
      x: backingStartX
      y: backingStartY
      width: previewWidth
      height: previewHeight

      PhotoComponent {
        id: preview

        width: insideShadow.width
        height: insideShadow.height
        anchors.centerIn: parent

        visible: (mediaSource) ? true : false

        isCropped: true
        isPreview: true
      }

      Image {
        id: insideShadow

        anchors.centerIn: parent

        source: "../img/photo-preview-inside-shadow.png"
        cache: true
      }
    }
  }
}

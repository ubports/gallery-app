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

  PhotoComponent {
    id: preview

    anchors.fill: parent

    visible: (mediaSource) ? true : false

    isCropped: true
    isPreview: true
  }

  Image {
    id: mat

    anchors.centerIn: parent

    source: "../img/photo-mat.png"
    cache: true
  }
}

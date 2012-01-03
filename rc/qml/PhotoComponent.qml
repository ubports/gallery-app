/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 1.1

Rectangle {
  property variant mediaSource
  property bool isCropped: false
  property bool isPreview: false
  
  clip: true
  
  Image {
    id: image
    objectName: "image"
    
    source: {
      if (!parent.mediaSource)
        return "";
      
      return isPreview ? mediaSource.previewPath : mediaSource.path
    }
    
    anchors.centerIn: parent
    
    transform: Scale {
      origin.x: width / 2
      origin.y: height / 2
      xScale: (mediaSource && !isPreview && mediaSource.orientationMirrored) ? -1.0 : 1.0
    }
    rotation: (mediaSource && !isPreview) ? mediaSource.orientationRotation : 0.0
    
    // If image is rotated by transform/rotation properties, also rotate its
    // dimensions so the loader scales it properly
    width: (!isPreview && mediaSource && mediaSource.isRotated) ? parent.height : parent.width
    height: (!isPreview && mediaSource && mediaSource.isRotated) ? parent.width : parent.height
    
    sourceSize.width: (width <= 1024) ? (width * 2) : width
    
    asynchronous: true
    cache: true
    smooth: true
    fillMode: isCropped ? Image.PreserveAspectCrop : Image.PreserveAspectFit
  }
}

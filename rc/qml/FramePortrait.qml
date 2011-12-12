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
  id: frame_portrait
  objectName: "frame_portrait"
  
  property string image_source
  property int frame_gutter
  property int image_gutter
  
  anchors.margins: frame_gutter
  
  border.width: 3
  border.color: "#657CA9"
  
  Image {
    id: image
    objectName: "image"
    
    anchors.centerIn: parent
    
    width: parent.width - (image_gutter * 2)
    height: parent.height - (image_gutter * 2)
    
    source: image_source
    
    asynchronous: true
    cache: true
    smooth: true
    fillMode: Image.PreserveAspectCrop
    clip: true
  }
}

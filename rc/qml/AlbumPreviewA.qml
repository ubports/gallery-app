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
  id: album_preview_a
  objectName: "album_preview_a"
  
  property string left_source
  property string right_source
  
  property int frame_gutter: 24
  property int image_gutter: 24
  
  FramePortrait {
    id: left
    objectName: "left"
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    
    width: (parent.width / 2) - (frame_gutter * 2) - 1
    
    image_source: left_source
    frame_gutter: parent.frame_gutter
    image_gutter: parent.image_gutter
  }
  
  Rectangle {
    id: divider
    objectName: "divider"
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: left.right
    anchors.right: right.left
    
    width: 1
    color: "#657CA9"
  }
  
  FramePortrait {
    id: right
    objectName: "right"
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    
    width: (parent.width / 2) - (frame_gutter * 2) - 1
    
    image_source: right_source
    frame_gutter: parent.frame_gutter
    image_gutter: parent.image_gutter
  }
}

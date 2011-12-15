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

Row {
  id: album_preview_b
  objectName: "album_preview_b"
  
  property variant preview_list
  property int frame_gutter: 24
  property int image_gutter: 24
  property int border_width: 3
  
  Column {
    width: ((parent.width / 2) - (album_preview_b.frame_gutter * 2)) - album_preview_b.border_width
    height: parent.height
    
    FramePortrait {
      id: top
      objectName: "top"
      
      width: parent.width
      height: ((parent.height / 2) - (album_preview_b.frame_gutter * 2)) - border_width
      
      image_source: preview_list[0]
      frame_gutter: album_preview_b.frame_gutter
      image_gutter: album_preview_b.image_gutter
      border.width: album_preview_b.border_width
    }
    
    Rectangle {
      id: hdivider
      objectName: "hdivider"
      
      width: parent.width
      height: album_preview_b.border_width
      color: "#657CA9"
    }
    
    FramePortrait {
      id: bottom
      objectName: "bottom"
      
      width: parent.width
      height: ((parent.height / 2) - (album_preview_b.frame_gutter * 2)) - border_width
      
      image_source: preview_list[1]
      frame_gutter: album_preview_b.frame_gutter
      image_gutter: album_preview_b.image_gutter
      border_width: album_preview_b.border_width
    }
  }
  
  Rectangle {
    id: vdivider
    objectName: "vdivider"
    
    width: top.border_width
    height: parent.height - album_preview_b.border_width
    color: "#657CA9"
  }
  
  FramePortrait {
    id: right
    objectName: "right"
    
    width: ((parent.width / 2) - (album_preview_b.frame_gutter * 2)) - border_width
    height: parent.height - border_width
    
    image_source: preview_list[2]
    frame_gutter: album_preview_b.frame_gutter
    image_gutter: album_preview_b.image_gutter
    border_width: album_preview_b.border_width
  }
}

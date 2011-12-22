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
  id: album_preview_a
  objectName: "album_preview_a"
  
  property variant preview_list
  property int frame_gutter: 24
  property int image_gutter: 24
  property int border_width: 3
  
  FramePortrait {
    id: left
    objectName: "left"
    
    width: (parent.width / 2) - (frame_gutter * 2) - border_width
    height: parent.height - border_width
    
    image_source: { (preview_list != null) ? preview_list[0] : ""; }
    frame_gutter: parent.frame_gutter
    image_gutter: parent.image_gutter
    border_width: parent.border_width
  }
  
  Rectangle {
    id: divider
    objectName: "divider"
    
    width: left.border_width
    height: parent.height
    color: "#657CA9"
  }
  
  FramePortrait {
    id: right
    objectName: "right"
    
    width: (parent.width / 2) - (frame_gutter * 2) - border_width
    height: parent.height - border_width
    
    image_source: { (preview_list != null) ? preview_list[1] : ""; }
    frame_gutter: parent.frame_gutter
    image_gutter: parent.image_gutter
    border_width: parent.border_width
  }
}

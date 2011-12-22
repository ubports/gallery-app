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
  property variant modelElement
  property variant preview_list: modelElement.preview_list
  
  width: 388
  height: 252
  
  onPreview_listChanged: {
    // if the preview_list changed, force the loader to reload the same
    // qml_rc (which is then populated with the new previews)
    loader.source = "";
    loader.source = modelElement.qml_rc;
  }
  
  Loader {
    id: loader
    
    width: parent.width
    height: parent.height
    clip: true
    
    source: modelElement.qml_rc
    
    onItemChanged: {
      if (item != null) {
        item.preview_list = parent.preview_list;
        item.image_gutter = 8;
        item.frame_gutter = 0;
      }
    }
  }
  
  Text {
    height: 24
    
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: loader.bottom
    anchors.topMargin: 12
    anchors.bottomMargin: 12
    
    color: "#657CA9"
    
    text: modelElement.album_name
    smooth: true
  }
}
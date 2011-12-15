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
  id: album_viewer
  objectName: "album_viewer"
  
  signal exit_viewer()
  
  anchors.fill: parent
  
  color: "#444444"
  
  TopBar {
    id: topbar
    objectName: "topbar"
  }
  
  ListView {
    id: template_pager
    objectName: "template_pager"
    
    anchors.top: topbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    
    model: context_album_model
    currentIndex: context_start_index
    
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: width * 2
    flickDeceleration: 50
    keyNavigationWraps: true
    highlightMoveSpeed: 2000.0
    
    delegate: Loader {
      id: loader
      
      width: template_pager.width
      height: template_pager.height
      
      source: qml_rc
      
      onLoaded: {
        item.preview_list = media_path_list;
        item.width = template_pager.width;
        item.height = template_pager.height;
        item.image_gutter = 24;
        item.frame_gutter = 0;
      }
    }
    
    MouseArea {
      anchors.fill: parent
      
      onDoubleClicked: {
        exit_viewer();
      }
    }
  }
}

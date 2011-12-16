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
    
    visible: true
    
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
  }
  
  Checkerboard {
    id: photos_checkerboard
    objectName: "photos_checkerboard"
    
    anchors.top: topbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    visible: false
    
    allow_selection: false
    
    checkerboardModel: context_media_model
    checkerboardDelegate: Image {
      source: modelData.preview_path
      
      anchors.centerIn: parent
      
      width: parent.width
      height: parent.height
      
      asynchronous: true
      cache: true
      smooth: true
      fillMode: Image.PreserveAspectCrop
      clip: true
    }
  }
  
  NavToolbar {
    z: 10
    anchors.bottom: parent.bottom
    
    translucent: true
    
    ReturnButton {
      id: return_button
      objectName: "return_button"
      
      x: 48
      
      show_title: false
      
      onPressed: album_viewer.exit_viewer()
    }
    
    NavButton {
      id: grid_button
      objectName: "grid_button"
      
      anchors.left: return_button.right
      
      visible: true
      
      title: "grid"
      
      onPressed: {
        template_pager.visible = false;
        photos_checkerboard.visible = true;
        
        grid_button.visible = false;
        pages_button.visible = true;
      }
    }
    
    NavButton {
      id: pages_button
      objectName: "pages_button"
      
      anchors.left: return_button.right
      
      visible: false
      
      title: "pages"
      
      onPressed: {
        template_pager.visible = true;
        photos_checkerboard.visible = false;
        
        grid_button.visible = true;
        pages_button.visible = false;
      }
    }
  }
}

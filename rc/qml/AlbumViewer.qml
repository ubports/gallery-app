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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1

Rectangle {
  id: album_viewer
  objectName: "album_viewer"
  
  signal exit_viewer()
  
  anchors.fill: parent

  AlbumViewMasthead {
    id: masthead
    objectName: "masthead"

    albumName: template_pager.albumName

    x: 0
    y: 0
    width: parent.width

    onViewModeChanged: {
      if (isTemplateView) {
        template_pager.visible = true;
        grid_checkerboard.visible = false;
      } else {
        template_pager.visible = false;
        grid_checkerboard.visible = true;
      }
    }
  }
  
  ListView {
    id: template_pager
    objectName: "template_pager"
    
    property string albumName

    anchors.top: masthead.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    
    model: ctx_album_viewer_album_model
    
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
        item.mediaSourceList = mediaSourceList;
        item.width = template_pager.width;
        item.height = template_pager.height;
        item.gutter = 24;
        template_pager.albumName = album_name;
      }
    }
  }
  
  Checkerboard {
    id: grid_checkerboard
    objectName: "grid_checkerboard"
    
    anchors.top: masthead.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    visible: false
    
    allow_selection: false
    
    checkerboardModel: ctx_album_viewer_media_model
    checkerboardDelegate: PhotoComponent {
      anchors.centerIn: parent
      
      width: parent.width
      height: parent.height
      
      mediaSource: modelData.media_source
      isCropped: true
      isPreview: true
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
      
      onPressed: album_viewer.exit_viewer();
    }    
  }
}

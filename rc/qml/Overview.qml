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
import org.yorba.qt.qmlmediamodel 1.0
import org.yorba.qt.qmlalbummodel 1.0

Rectangle {
  id: overview
  objectName: "overview"
  
  signal create_album_from_selected()
  
  anchors.fill: parent
  
  TopBar {
    id: topbar
    objectName: "topbar"
  }
  
  NavToolbar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: topbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    
    BinaryTabGroup {
      id: album_view_switcher
      objectName: "album_view_switcher"
      
      x: parent.width / 2 - width / 2
      y: 4
      
      tab0_title: "Albums"
      tab1_title: "Photos"
      
      state: "tab1_active"
      
      onTab0_activated: {
        albums_checkerboard.visible = true;
        photos_checkerboard.visible = false;
      }
      
      onTab1_activated: {
        albums_checkerboard.visible = false;
        photos_checkerboard.visible = true;
      }
    }
    
    NavButton {
      id: deselect
      objectName: "deselect"
      
      anchors.right:  cancel_selecting.left
      
      title: "deselect"
      visible: photos_checkerboard.in_selection_mode
      
      onPressed: {
        photos_checkerboard.unselect_all();
      }
    }
    
    NavButton {
      id: cancel_selecting
      objectName: "cancel_selecting"
      
      anchors.right:  parent.right
      
      title: "done"
      visible: photos_checkerboard.in_selection_mode
      
      onPressed: {
        photos_checkerboard.state = "normal";
        photos_checkerboard.unselect_all();
      }
    }
  }
  
  Checkerboard {
    id: photos_checkerboard
    objectName: "photos_checkerboard"
    
    anchors.top: navbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    visible: true
    allow_selection: true
    
    checkerboardModel: photosGridModel
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
  
  Checkerboard {
    id: albums_checkerboard
    objectName: "albums_checkerboard"
    
    anchors.top: navbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    widthSansStroke: 412
    heightSansStroke: 312
    widthWithStroke: 388
    heightWithStroke: 288
    
    visible: false
    allow_selection: false
    
    checkerboardModel: albumsGridModel
    checkerboardDelegate: Rectangle {
      AlbumPreviewA {
        id: preview
        
        width: 388
        height: 252
        
        left_source: modelData.preview_a
        right_source: modelData.preview_b
        
        image_gutter: 8
        frame_gutter: 0
      }
      
      Text {
        height: 24
        
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: preview.bottom
        anchors.topMargin: 12
        anchors.bottomMargin: 12
        
        color: "#657CA9"
        
        text: modelData.album_name
        smooth: true
      }
    }
  }
  
  NavToolbar {
    id: toolbar
    objectName: "toolbar"
    
    z: 10
    visible: photos_checkerboard.in_selection_mode
    anchors.bottom: parent.bottom
    
    NavButton {
      id: create_album
      objectName: "create_album"
      
      anchors.right: parent.right
      
      title: "album"
      
      onPressed: {
        overview.create_album_from_selected();
        photos_checkerboard.state = "normal";
        photos_checkerboard.unselect_all();
      }
    }
  }
}

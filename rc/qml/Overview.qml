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
  id: overview
  objectName: "overview"
  
  signal create_album()
  signal create_album_from_selected()
  signal popupAlbumPicked(int album_number)
  
  anchors.fill: parent
  
  NavToolbar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    NavButton {
      id: createAlbum
      objectName: "createAlbum"
      
      anchors.left: parent.left
      
      title: "create"
      visible: albums_checkerboard.visible
      
      onPressed: create_album()
    }
    
    BinaryTabGroup {
      id: album_view_switcher
      objectName: "album_view_switcher"
      
      x: parent.width / 2 - width / 2
      y: 4
      
      tab0_title: "Albums"
      tab1_title: "Photos"
      
      state: "tab1_active"
      
      visible: !photos_checkerboard.in_selection_mode
      
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
      
      anchors.right: cancel_selecting.left
      
      title: "deselect"
      visible: photos_checkerboard.selected_count > 0
      
      onPressed: {
        if (album_picker.visible) {
          album_picker.visible = false;
          return;
        }
        
        photos_checkerboard.unselect_all();
      }
    }
    
    NavButton {
      id: cancel_selecting
      objectName: "cancel_selecting"
      
      anchors.right: parent.right
      
      title: "done"
      visible: photos_checkerboard.in_selection_mode
      
      onPressed: {
        if (album_picker.visible) {
          album_picker.visible = false;
          return;
        }
        
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
    
    checkerboardModel: ctx_overview_media_model
    checkerboardDelegate: PhotoComponent {
      mediaSource: modelData.media_source
      isCropped: true
      isPreview: true
    }

    MouseArea {
      id: mouse_blocker
      objectName: "mouse_blocker"

      anchors.fill: parent

      onClicked: {
        if (album_picker.visible) {
          album_picker.visible = false;
          return;
        }
      }

      visible: false
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
    
    checkerboardModel: ctx_overview_albums_model
    checkerboardDelegate: AlbumPreviewComponent {
      qmlRC: modelData.album.pages[modelData.album.currentPage].qmlRC
      albumName: modelData.album.name
      mediaSourceList: modelData.album.pages[modelData.album.currentPage].mediaSourceList
    }
  }

  AlbumPickerPopup {
    id: album_picker
    objectName: "album_picker"

    y: parent.height - height - toolbar.height
    x: add_to_album_button.x + add_to_album_button.width / 2 - width + 16

    visible: false

    designated_model: ctx_album_picker_model

    onNewAlbumRequested: {
      overview.create_album_from_selected();
      photos_checkerboard.state = "normal";
      photos_checkerboard.unselect_all();
      visible = false
    }

    onSelected: {
      overview.popupAlbumPicked(album_number);
      photos_checkerboard.state = "normal";
      photos_checkerboard.unselect_all();
      visible = false;
    }

    onVisibleChanged: {
      mouse_blocker.visible = visible
    }
  }
  
  NavToolbar {
    id: toolbar
    objectName: "toolbar"
    
    z: 10
    visible: photos_checkerboard.in_selection_mode
    anchors.bottom: parent.bottom
    
    Text {
      anchors.centerIn: parent
      
      text: "Select photos or movieclip(s)"
      visible: photos_checkerboard.selected_count == 0
    }

    AddToAlbumButton {
      id: add_to_album_button
      objectName: "add_to_album_button"

      visible: photos_checkerboard.selected_count > 0

      onPressed: album_picker.visible = !album_picker.visible;
    }
  }
}

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
  signal addToAlbum()
  signal popupAlbumPicked(int album_number)
  signal createAlbumFromSelected()
  
  anchors.fill: parent

  AlbumViewMasthead {
    id: masthead
    objectName: "masthead"

    albumName: template_pager.albumName

    areItemsSelected: grid_checkerboard.selected_count > 0

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

    onSelectionInteractionCompleted: {
      grid_checkerboard.state = "normal";
      grid_checkerboard.unselect_all();
    }

    onDeselectAllRequested: {
      grid_checkerboard.unselect_all();
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
    
    allow_selection: true
    
    checkerboardModel: ctx_album_viewer_media_model
    checkerboardDelegate: PhotoComponent {
      anchors.centerIn: parent
      
      width: parent.width
      height: parent.height
      
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

    onIn_selection_modeChanged: {
      masthead.isSelectionInProgress = in_selection_mode
    }
  }

  AlbumPickerPopup {
    id: album_picker
    objectName: "album_picker"

    y: parent.height - height - toolbar.height
    x: add_to_album_button.x + add_to_album_button.width / 2 - width + 16

    designated_model: ctx_album_picker_model

    visible: false

    onNewAlbumRequested: {
      album_viewer.createAlbumFromSelected();
      grid_checkerboard.state = "normal";
      grid_checkerboard.unselect_all();
      visible = false
    }

    onSelected: {
      album_viewer.popupAlbumPicked(album_number);
      grid_checkerboard.state = "normal";
      grid_checkerboard.unselect_all();
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
    anchors.bottom: parent.bottom
    
    translucent: true
    
    ReturnButton {
      id: return_button
      objectName: "return_button"
      
      x: 48
      
      show_title: false
      
      onPressed: {
        album_viewer.exit_viewer();
        album_picker.visible = false
        masthead.isSelectionInProgress = false
        masthead.areItemsSelected = false
        grid_checkerboard.state = "normal";
        grid_checkerboard.unselect_all();
      }
    }

    AddToAlbumButton {
      id: add_to_album_button
      objectName: "add_to_album_button"

      visible: grid_checkerboard.selected_count > 0

      onPressed: {
        album_picker.visible = !album_picker.visible
	  }
    }

    NavButton {
      id: addToAlbumButton
      objectName: "addToAlbumButton"
      
      anchors.right: parent.right
      
      title: "add photos"
      
      onPressed: addToAlbum()
    }
  }
}

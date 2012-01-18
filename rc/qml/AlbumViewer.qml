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
import Gallery 1.0

Rectangle {
  id: album_viewer
  objectName: "album_viewer"
  
  property variant album
  
  anchors.fill: parent

  AlbumViewMasthead {
    id: masthead
    objectName: "masthead"

    albumName: template_pager.albumName

    areItemsSelected: gridCheckerboard.selectedCount > 0

    x: 0
    y: 0
    width: parent.width

    onViewModeChanged: {
      if (isTemplateView) {
        template_pager.visible = true;
        gridCheckerboard.visible = false;
      } else {
        template_pager.visible = false;
        gridCheckerboard.visible = true;
      }
    }

    onSelectionInteractionCompleted: {
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
    }

    onDeselectAllRequested: {
      gridCheckerboard.unselectAll();
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
    
    visible: true
    
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: width * 2
    flickDeceleration: 50
    keyNavigationWraps: true
    highlightMoveSpeed: 2000.0
    
    model: (album) ? album.pages : null
    
    delegate: Loader {
      id: loader
      
      width: template_pager.width
      height: template_pager.height
      
      source: qmlRC
      
      onLoaded: {
        item.mediaSourceList = mediaSourceList;
        item.width = template_pager.width;
        item.height = template_pager.height;
        item.gutter = 24;
        template_pager.albumName = owner.name;
      }
    }
  }
  
  Checkerboard {
    id: gridCheckerboard
    objectName: "gridCheckerboard"
    
    anchors.top: masthead.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    visible: false
    
    allowSelection: true
    
    checkerboardModel: MediaCollectionModel {
      forCollection: album
    }
    
    checkerboardDelegate: PhotoComponent {
      anchors.centerIn: parent
      
      width: parent.width
      height: parent.height
      
      mediaSource: modelData.mediaSource
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

    onInSelectionModeChanged: {
      masthead.isSelectionInProgress = inSelectionMode
    }
    
    onActivated: navStack.switchToPhotoViewer(object, checkerboardModel)
  }

  AlbumPickerPopup {
    id: album_picker
    objectName: "album_picker"

    y: parent.height - height - toolbar.height
    x: add_to_album_button.x + add_to_album_button.width / 2 - width + 16

    designated_model: AlbumCollectionModel {
    }

    visible: false

    onNewAlbumRequested: {
      gridCheckerboard.checkerboardModel.createAlbumFromSelected();
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
      visible = false
    }

    onSelected: {
      album.addSelectedMediaSources(gridCheckerboard.checkerboardModel);
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
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
        album_picker.visible = false
        masthead.isSelectionInProgress = false
        masthead.areItemsSelected = false
        gridCheckerboard.state = "normal";
        gridCheckerboard.unselectAll();
        
        navStack.goBack();
      }
    }

    AddToAlbumButton {
      id: add_to_album_button
      objectName: "add_to_album_button"

      visible: gridCheckerboard.selectedCount > 0

      onPressed: {
        album_picker.visible = !album_picker.visible
      }
    }

    NavButton {
      id: addToAlbumButton
      objectName: "addToAlbumButton"
      
      anchors.right: parent.right
      
      title: "add photos"
      
      onPressed: navStack.switchToMediaSelector(album)
    }
  }
}

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
  id: overview
  objectName: "overview"
  
  anchors.fill: parent
  
  GalleryOverviewNavigationBar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    onAddCreateOperationButtonPressed: {
      if (album_view_switcher.state == "tab0_active")
        navStack.switchToMediaSelector();
    }
    
    BinaryTabGroup {
      id: album_view_switcher
      objectName: "album_view_switcher"
      
      x: parent.width / 2 - width / 2
      y: 4
      
      tab0_title: "Albums"
      tab1_title: "Events"
      
      state: "tab1_active"
      
      visible: !photos_checkerboard.inSelectionMode
      
      onTab0_activated: {
        albums_checkerboard.visible = true;
        photos_checkerboard.visible = false;
      }
      
      onTab1_activated: {
        albums_checkerboard.visible = false;
        photos_checkerboard.visible = true;
      }
    }
    
    GallerySecondaryPushButton {
      id: deselect
      objectName: "deselect"
      
      anchors.right: cancel_selecting.left
      anchors.rightMargin: 16
      anchors.verticalCenter: parent.verticalCenter

      title: "deselect"

      visible: photos_checkerboard.selectedCount > 0
      
      onPressed: {
        if (album_picker.state == "shown") {
          album_picker.state = "hidden";
          return;
        }
        
        photos_checkerboard.unselectAll();
      }
    }
    
    GalleryPrimaryPushButton {
      id: cancel_selecting
      objectName: "cancel_selecting"
      
      anchors.right: parent.right
      anchors.rightMargin: 16
      anchors.verticalCenter: parent.verticalCenter

      title: "done"

      visible: photos_checkerboard.inSelectionMode
      
      onPressed: {
        if (album_picker.state == "shown") {
          album_picker.state = "hidden";
          return;
        }

        photos_checkerboard.state = "normal";
        photos_checkerboard.unselectAll();
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
    allowSelection: true
    
    checkerboardModel: EventCollectionModel {
    }
    
    property variant photoViewerModel: MediaCollectionModel {
    }
    
    checkerboardDelegate: Item {
      PhotoComponent {
        anchors.fill: parent
        
        visible: modelData.typeName == "MediaSource"
        
        mediaSource: (visible) ? modelData.mediaSource : null
        isCropped: true
        isPreview: true
      }
      
      Text {
        anchors.fill: parent
        
        color: "mediumBlue"
        visible: modelData.typeName == "QmlEventMarker"
        
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        
        text: (visible) ? modelData.object.prettyDate : ""
      }
    }
    
    MouseArea {
      id: mouse_blocker
      objectName: "mouse_blocker"

      anchors.fill: parent

      onClicked: {
        if (album_picker.state == "shown") {
          album_picker.state = "hidden";
          return;
        }
      }

      visible: album_picker.state == "shown"
    }
    
    onActivated: {
      if (objectModel.typeName == "MediaSource")
        navStack.switchToPhotoViewer(object, photoViewerModel);
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
    allowSelection: false
    
    checkerboardModel: AlbumCollectionModel {
    }
    
    checkerboardDelegate: AlbumPreviewComponent {
      albumPage: (modelData.album.currentPage >= 0) ? modelData.album.pages[modelData.album.currentPage] : null
    }
    
    onActivated: navStack.switchToAlbumViewer(object)
  }

  AlbumPickerPopup {
    id: album_picker
    objectName: "album_picker"

    y: parent.height - height - toolbar.height
    x: toolbar.albumOperationsPopupX - width

    state: "hidden"

    designated_model: AlbumCollectionModel {
    }

    onNewAlbumRequested: {
      photos_checkerboard.checkerboardModel.createAlbumFromSelected();
      photos_checkerboard.state = "normal";
      photos_checkerboard.unselectAll();
      state = "hidden"
    }

    onSelected: {
      album.addSelectedMediaSources(photos_checkerboard.checkerboardModel);
      photos_checkerboard.state = "normal";
      photos_checkerboard.unselectAll();
      state = "hidden"
    }
  }
  
  GalleryStatusBar {
    id: statusBar
    objectName: "statusBar"
    
    z: 10
    anchors.bottom: parent.bottom

    visible: (photos_checkerboard.inSelectionMode &&
      photos_checkerboard.selectedCount == 0)
    
    statusText: "Select photos or movieclip(s)"
  }

  GalleryStandardToolbar {
    id: toolbar
    objectName: "toolbar"

    hasReturnButton: false
    isTranslucent: true
    background: "lightBlue"

    z: 10
    anchors.bottom: parent.bottom

    visible: (photos_checkerboard.inSelectionMode &&
      photos_checkerboard.selectedCount > 0)

    onAlbumOperationsButtonPressed: { album_picker.flipVisibility(); }

  }
}

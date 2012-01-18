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
import Gallery 1.0

Rectangle {
  id: mediaSelector
  objectName: "mediaSelector"
  
  property variant album
  
  anchors.fill: parent
  
  GalleryOverviewNavigationBar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    GallerySecondaryPushButton {
      id: deselectButton
      objectName: "deselectButton"
      
      anchors.right: doneButton.left
      anchors.rightMargin: 16
      anchors.verticalCenter: parent.verticalCenter

      visible: mediaCheckerboard.selectedCount > 0
      
      title: "deselect"
      
      onPressed: mediaCheckerboard.checkerboardModel.unselectAll()
    }
    
    GalleryPrimaryPushButton {
      id: doneButton
      objectName: "doneButton"
      
      anchors.right: parent.right
      anchors.rightMargin: 16
      anchors.verticalCenter: parent.verticalCenter
      
      title: "done"
      
      onPressed: {
        if (album)
          album.addSelectedMediaSources(mediaCheckerboard.checkerboardModel);
        else
          album = mediaCheckerboard.checkerboardModel.createAlbumFromSelected();

        mediaCheckerboard.unselectAll();

        navStack.goBack()
      }
    }
  }
  
  Checkerboard {
    id: mediaCheckerboard
    objectName: "mediaCheckerboard"
    
    anchors.top: navbar.bottom
    anchors.left: navbar.left
    anchors.right: navbar.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    allowSelection: true
    state: "selecting"
    
    checkerboardModel: MediaCollectionModel {
    }
    
    checkerboardDelegate: PhotoComponent {
      mediaSource: modelData.mediaSource
      isCropped: true
      isPreview: true
    }
  }
  
  GalleryStatusBar {
    anchors.bottom: parent.bottom

    statusText: (mediaCheckerboard.selectedCount == 0) ?
      "Select photos or movieclip(s) to add" : ""
  }
}

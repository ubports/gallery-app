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

import QtQuick 1.0
import Gallery 1.0

Rectangle {
  id: tablet_surface
  objectName: "tablet_surface"
  
  width: 1280
  height: 776
  color: "white"
  
  Overview {
    id: overview
    objectName: "overview"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: true
  }
  
  PhotoViewer {
    id: photo_viewer
    objectName: "photo_viewer"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: false
  }
  
  AlbumViewer {
    id: album_viewer
    objectName: "album_viewer"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: false
  }
  
  MediaSelector {
    id: mediaSelector
    objectName: "mediaSelector"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: false
  }
  
  Stack {
    id: navStack
    objectName: "navStack"
    
    function switchToPage(page) {
      if (!isEmpty)
        top.visible = false;
      
      page.visible = true;
      push(page);
    }
    
    function goBack() {
      if (isEmpty)
        return;
      
      top.visible = false;
      pop();
      
      if (!isEmpty)
        top.visible = true;
    }
  }
  
  function goBack() {
    navStack.goBack();
  }
  
  function switchToPhotoViewer(photo, model) {
    photo_viewer.photo = photo;
    photo_viewer.model = model;
    
    navStack.switchToPage(photo_viewer);
  }
  
  function switchToAlbumViewer(album) {
    album_viewer.album = album;
    
    navStack.switchToPage(album_viewer);
  }
  
  function switchToMediaSelector(album) {
    mediaSelector.album = album;
    
    navStack.switchToPage(mediaSelector);
  }
  
  Component.onCompleted: {
    navStack.switchToPage(overview);
  }
}

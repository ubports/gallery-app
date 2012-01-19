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
  id: photoViewer
  objectName: "photoViewer"
  
  // NOTE: These properties should be treated as read-only, as setting them
  // individually can lead to bogus results.  Use setCurrentPhoto() to
  // initialize the view.
  property variant photo: null
  property variant model: null
  
  function setCurrentPhoto(photo, model) {
    photoViewer.photo = photo;
    photoViewer.model = model;
    
    imagePager.positionViewAtIndex(model.indexOf(photo), 0);
    imagePager.currentIndex = model.indexOf(photo);
  }
  
  color: "#444444"
  
  AlbumPickerPopup {
    id: albumPicker
    objectName: "albumPicker"

    y: parent.height - height - toolbar.height
    x: toolbar.albumOperationsPopupX - width

    state: "hidden";

    designated_model: AlbumCollectionModel {
    }

    onSelected: {
      album.addMediaSource(photo);
      state = "hidden"
    }
    
    onNewAlbumRequested: {
      designated_model.createAlbum(photo);
      state = "hidden"
    }
  }
  
  ListView {
    id: imagePager
    objectName: "imagePager"
    
    z: 0
    anchors.fill: parent
    
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: width * 2
    flickDeceleration: 50
    keyNavigationWraps: true
    highlightMoveSpeed: 2000.0
    
    model: parent.model
    
    delegate: PhotoComponent {
      width: imagePager.width
      height: imagePager.height
      
      color: "#444444"
      
      mediaSource: model.mediaSource
    }
    
    // don't allow flicking while albumPicker is visible
    interactive: albumPicker.state == "hidden"
    
    MouseArea {
      anchors.fill: parent
      
      onClicked: {
        // dismiss album picker if up without changing chrome state
        if (albumPicker.state == "shown") {
          albumPicker.state = "hidden";
          
          return;
        }
        
        // reverse album chrome's visible
        chromeWrapper.state = (chromeWrapper.state == "shown")
          ? "hidden" : "shown";
      }
    }
    
    function updateCurrentIndex() {
      // Add one to ensure the hit-test is inside the delegate's boundaries
      currentIndex = indexAt(contentX + 1, contentY + 1);
      if (model)
        photo = model.getAt(currentIndex);
    }
    
    onModelChanged: {
      updateCurrentIndex();
    }
    
    onVisibleChanged: {
      updateCurrentIndex();
    }
    
    onCountChanged: {
      updateCurrentIndex();
    }
    
    onMovementEnded: {
      updateCurrentIndex();
    }
  }
  
  Rectangle {
    id: chromeWrapper
    objectName: "chromeWrapper"
    
    states: [
      State { name: "shown";
        PropertyChanges { target: chromeWrapper; opacity: 1; } },

      State { name: "hidden";
        PropertyChanges { target: chromeWrapper; opacity: 0; } }
    ]
    
    transitions: [
      Transition { from: "*"; to: "*";
        NumberAnimation { properties: "opacity"; easing.type:
                          Easing.InQuad; duration: 200; } }
    ]
    
    state: "hidden"
    
    z: 10
    anchors.fill: parent
    
    color: "transparent"
    
    GalleryStandardToolbar {
      id: toolbar
      objectName: "toolbar"

      isTranslucent: true
      opacity: 0.8 /* override default opacity when translucent == true of 0.9
                      for better compliance with the spec */

      z: 10
      anchors.bottom: parent.bottom

      onAlbumOperationsButtonPressed: albumPicker.flipVisibility();

      onReturnButtonPressed: {
        chromeWrapper.state = "hidden";
        albumPicker.state = "hidden";
        navStack.goBack();
      }
    }
    
    ViewerNavigationButton {
      is_forward: false
      
      x: 12
      y: 2 * parent.height / 3
      z: 20
      
      visible: !imagePager.atXBeginning
      
      onPressed: imagePager.decrementCurrentIndex()
    }
    
    ViewerNavigationButton {
      is_forward: true
      
      x: parent.width - width - 12
      y: 2 * parent.height / 3
      z: 20
      
      visible: !imagePager.atXEnd
      
      onPressed: imagePager.incrementCurrentIndex()
    }
  }
}


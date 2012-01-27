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
    
    imagePager.pageTo(model.indexOf(photo));
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
  
  Pager {
    id: imagePager
    objectName: "imagePager"
    
    z: 0
    
    model: parent.model
    
    delegate: PhotoComponent {
      width: imagePager.width
      height: imagePager.height
      
      color: "#444444"

      isZoomable: true;
      
      mediaSource: model.mediaSource
    }
    
    // Don't allow flicking while albumPicker is visible or the image is zoomed.
    // When images are zoomed, mouse drags should pan, not flick.
    interactive: (albumPicker.state == "hidden") && (currentItem != null) &&
      (currentItem.state != "zoomed")
    
    onCurrentIndexChanged: {
      if (model)
        photo = model.getAt(currentIndex);
    }
    
    MouseArea {
      anchors.fill: parent

      property bool isDragInProgress: false;
      property int dragStartX: -1;
      property int dragStartY: -1;

      Timer {
        id: chromeFadeWaitClock

        interval: 150
        running: false

        onTriggered: {
          chromeWrapper.state = (chromeWrapper.state == "shown") ?
            "hidden" : "shown";
        }
      }
      
      onClicked: {
        if (parent.currentItem.state != "zoomed")
          chromeFadeWaitClock.restart();
      }

      onPressed: {
        if (parent.currentItem.state != "zoomed")
          return;

        isDragInProgress = true;

        dragStartX = mouse.x;
        dragStartY = mouse.y;

        parent.currentItem.setZoomFocus(
              parent.currentItem.getImageTranslation());
      }

      onPositionChanged: {
        if (isDragInProgress) {
          var deltaX = mouse.x - dragStartX;
          var deltaY = mouse.y - dragStartY;

          parent.currentItem.pan(parent.currentItem.zoomFocusX + deltaX,
            parent.currentItem.zoomFocusY + deltaY);
        }
      }

      onReleased: {
        isDragInProgress = false;
      }

      onDoubleClicked: {
        chromeFadeWaitClock.stop();

        // we don't permit zooming when the album picker popup is open
        if (albumPicker.state == "shown")
          return;

        chromeWrapper.state = "hidden";

        imagePager.currentItem.zoom(mouse.x, mouse.y);
      }
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
      
      onPressed: imagePager.pageBack()
    }
    
    ViewerNavigationButton {
      is_forward: true
      
      x: parent.width - width - 12
      y: 2 * parent.height / 3
      z: 20
      
      visible: !imagePager.atXEnd
      
      onPressed: imagePager.pageForward()
    }
  }
}


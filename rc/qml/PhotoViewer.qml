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
  id: photo_viewer
  objectName: "photo_viewer"
  
  color: "#444444"
  
  signal exit_viewer()
  
  property int visible_index
  
  AlbumPickerPopup {
    id: album_picker
    objectName: "album_picker"

    y: parent.height - height - toolbar.height
    x: add_to_album_button.x + add_to_album_button.width / 2 - width + 16

    visible: false

    designated_model: ctx_album_picker_model
  }
  
  ListView {
    id: image_pager
    objectName: "image_pager"
    
    z: 0
    anchors.fill: parent
    
    model: ctx_photo_viewer_photo_model
    
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: width * 2
    flickDeceleration: 50
    keyNavigationWraps: true
    highlightMoveSpeed: 2000.0
    
    delegate: PhotoComponent {
      width: image_pager.width
      height: image_pager.height
      
      color: "#444444"
      
      mediaSource: media_source
    }
    
    // don't allow flicking while album_picker is visible
    interactive: !album_picker.visible
    
    MouseArea {
      anchors.fill: parent
      
      onClicked: {
        // dismiss album picker if up without changing chrome state
        if (album_picker.visible) {
          album_picker.visible = false;
          
          return;
        }
        
        // reverse album chrome's visible
        chrome_wrapper.state = (chrome_wrapper.state == "shown")
          ? "hidden" : "shown";
      }
    }
    
    function updateVisibleIndex() {
      // Add one to ensure the hittest is in side the delegate's boundaries
      parent.visible_index = indexAt(contentX + 1, contentY + 1);
    }
    
    onModelChanged: {
      updateVisibleIndex();
    }
    
    onVisibleChanged: {
      updateVisibleIndex();
    }
    
    onCountChanged: {
      updateVisibleIndex();
    }
    
    onMovementEnded: {
      updateVisibleIndex();
    }
  }
  
  Rectangle {
    id: chrome_wrapper
    objectName: "chrome_wrapper"
    
    states: [
      State { name: "shown";
        PropertyChanges { target: chrome_wrapper; opacity: 1; } },

      State { name: "hidden";
        PropertyChanges { target: chrome_wrapper; opacity: 0; } }
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
    
    NavToolbar {
      id: toolbar
      objectName: "toolbar"

      z: 10
      anchors.bottom: parent.bottom
      
      translucent: true

      ReturnButton {
        x: 48
        
        show_title: false

        onPressed: {
          chrome_wrapper.state = "hidden";
          photo_viewer.exit_viewer();
        }
      }

      NavButton {
        id: add_to_album_button
        objectName: "add_to_album_button"

        title: "add to album"

        width: 98
        x: parent.width - 3 * width
        y: parent.height / 2 - height / 2

        onPressed: {
          album_picker.visible = !album_picker.visible;
        }
      }
    }
    
    ViewerNavigationButton {
      is_forward: false
      
      x: 12
      y: 2 * parent.height / 3
      z: 20
      
      visible: !image_pager.atXBeginning
      
      onPressed: image_pager.decrementCurrentIndex()
    }
    
    ViewerNavigationButton {
      is_forward: true
      
      x: parent.width - width - 12
      y: 2 * parent.height / 3
      z: 20
      
      visible: !image_pager.atXEnd
      
      onPressed: image_pager.incrementCurrentIndex()
    }
  }
}


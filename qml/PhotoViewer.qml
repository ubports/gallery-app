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
  
  anchors.fill: parent

  ListView {
    id: image_pager
    objectName: "image_pager"
    
    z: 0
    anchors.fill: parent
    
    model: viewer_model
    currentIndex: viewer_current_index
    
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    cacheBuffer: width * 2
    flickDeceleration: 50
    keyNavigationWraps: true
    highlightMoveSpeed: 2000.0
    
    delegate: Image {
      width: image_pager.width
      height: image_pager.height
      
      asynchronous: true
      cache: true
      fillMode: Image.PreserveAspectFit
      smooth: true
      
      source: path
    }
    
    MouseArea {
      anchors.fill: parent
      
      onClicked: { if (chrome_wrapper.state == "shown")
                     chrome_wrapper.state = "hidden"
                   else
                     chrome_wrapper.state = "shown"
                 }
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
    
    TopBar {
      id: topbar
      objectName: "topbar"
    }

    NavToolbar {
      z: 10
      anchors.bottom: parent.bottom
      
      translucent: true

      ReturnButton {
        x: 48
        
        show_title: false

        onPressed: photo_viewer.exit_viewer()
      }
    }
  }
}


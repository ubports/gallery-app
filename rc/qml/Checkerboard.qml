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

Rectangle {
  id: checkerboard
  objectName: "checkerboard"
  
  signal activated(int object_number)
  signal selection_toggled(int object_number)
  signal unselect_all()
  
  // The Checkerboard delegate requires these properties be set by the
  // model:
  // 
  // object_number
  // is_selected
  property variant checkerboardModel
  property Component checkerboardDelegate
  
  property int widthSansStroke: 206
  property int heightSansStroke: 156
  property int widthWithStroke: 198
  property int heightWithStroke: 148
  
  property bool in_selection_mode: false
  property bool allow_selection: true
  
  color: "white"
  clip: true
  
  state: "normal"
  states: [
    State { 
      name: "normal"
      PropertyChanges { target: checkerboard; in_selection_mode: false }
    },
    State { 
      name: "to-selecting" 
      PropertyChanges { target: checkerboard; in_selection_mode: true }
    },
    State { 
      name: "selecting"
      PropertyChanges { target: checkerboard; in_selection_mode: true }
    }
  ]
  
  GridView {
    id: grid
    objectName: "grid"
    
    width: parent.width
    height: parent.height
    
    // Dimensions without stroke
    cellWidth: widthSansStroke
    cellHeight: heightSansStroke
    
    model: parent.checkerboardModel
    
    delegate: Row {
      Rectangle {
        width: widthSansStroke
        height: heightSansStroke
        
        Loader {
          id: loader
          
          property variant modelData: model
          
          anchors.centerIn: parent
          
          width: widthWithStroke
          height: heightWithStroke
          
          sourceComponent: checkerboard.checkerboardDelegate
        }
        
        MouseArea {
          anchors.fill: loader
          
          onPressAndHold: {
            // Press-and-hold activates selection mode,
            // but need to differentiate in onReleased whether it's a mode
            // change or a selection/activation
            if (allow_selection && checkerboard.state == "normal")
              checkerboard.state = "to-selecting";
          }
          
          onReleased: {
            // See onPressAndHold for note on logic behind state changes
            if (checkerboard.state == "normal")
              checkerboard.activated(object_number)
            else if (allow_selection && checkerboard.state == "to-selecting")
              checkerboard.state = "selecting";
            else if (allow_selection && checkerboard.state == "selecting")
              checkerboard.selection_toggled(object_number);
          }
        }
        
        Image {
          source: "../img/selected-media.png"
          
          anchors.top: loader.top
          anchors.right: loader.right
          
          visible: is_selected
          
          asynchronous: true
          cache: true
          smooth: true
        }
      }
    }
  }
}

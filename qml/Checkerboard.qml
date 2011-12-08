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
  
  property bool in_selection_mode: false
  property bool allow_selection: true
  property variant checkerboardModel
  
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
    
    // spec calls for 206x156, 198x148 without stroke -- split the difference
    // here to not double up space in the middle gutters
    //
    // TODO: Verify that the edge gutters are properly spaced
    cellWidth: 202
    cellHeight: 152
    
    model: parent.checkerboardModel
    
    // The Checkerboard delegate requires these three properties be set by the
    // model:
    // 
    // object_number
    // preview_path (i.e. filesystem path to thumbnail)
    // is_selected
    delegate: Row {
      Rectangle {
        width: 202
        height: 152
        
        Rectangle {
          width: 198
          height: 148
          
          anchors.centerIn: parent
          
          Image {
            source: preview_path
            
            anchors.centerIn: parent
            
            asynchronous: true
            cache: true
            smooth: true
            
            MouseArea {
              anchors.fill: parent
              
              onPressAndHold: {
                // Press-and-hold activates selection mode,
                // but need to differentiate in onReleased whether it's a mode
                // change or a selection/activation
                if (allow_selection && checkerboard.state == "normal")
                  checkerboard.state = "to-selecting";
              }
              
              onReleased: {
                // See onPressAndHold for note on logic behind state changes
                if (!allow_selection)
                  return;
                
                if (checkerboard.state == "normal")
                  checkerboard.activated(object_number)
                else if (checkerboard.state == "to-selecting")
                  checkerboard.state = "selecting";
                else if (checkerboard.state == "selecting")
                  checkerboard.selection_toggled(object_number);
              }
            }
            
            Image {
              source: "selected-media.png"
              
              anchors.top: parent.top
              anchors.right: parent.right
              
              visible: is_selected
              
              asynchronous: true
              cache: true
              smooth: true
            }
          }
        }
      }
    }
  }
}

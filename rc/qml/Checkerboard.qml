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
  
  signal activated(variant object, variant objectModel)
  
  property alias checkerboardModel: grid.model
  property Component checkerboardDelegate
  
  property int widthSansStroke: 206
  property int heightSansStroke: 156
  property int widthWithStroke: 198
  property int heightWithStroke: 148
  
  property bool inSelectionMode: false
  property bool allowSelection: true
  property int selectedCount:
    (checkerboardModel != null) ? checkerboardModel.selectedCount : 0
  
  function unselectAll() {
    if (checkerboardModel != null)
      checkerboardModel.unselectAll();
  }
  
  color: "white"
  clip: true
  
  state: "normal"
  states: [
    State { 
      name: "normal"
      PropertyChanges { target: checkerboard; inSelectionMode: false }
    },
    State { 
      name: "to-selecting" 
      PropertyChanges { target: checkerboard; inSelectionMode: true }
    },
    State { 
      name: "selecting"
      PropertyChanges { target: checkerboard; inSelectionMode: true }
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
            if (allowSelection && checkerboard.state == "normal")
              checkerboard.state = "to-selecting";
          }
          
          onReleased: {
            // See onPressAndHold for note on logic behind state changes
            if (checkerboard.state == "normal")
              checkerboard.activated(object, model)
            else if (allowSelection && checkerboard.state == "to-selecting")
              checkerboard.state = "selecting";
            else if (allowSelection && checkerboard.state == "selecting")
              checkerboardModel.toggleSelection(object);
          }
        }
        
        Image {
          source: "../img/selected-media.png"
          
          anchors.top: loader.top
          anchors.right: loader.right
          
          visible: isSelected
          
          asynchronous: true
          cache: true
          smooth: true
        }
      }
    }
  }
}

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
  
  signal activated(variant object, variant objectModel, variant activatedRect)
  
  property alias model: grid.model
  property Component delegate
  
  property int widthSansStroke: 206
  property int heightSansStroke: 156
  property int widthWithStroke: 198
  property int heightWithStroke: 148
  
  property bool inSelectionMode: false
  property bool allowSelection: true
  property int selectedCount:
    (model != null) ? model.selectedCount : 0
  
  function unselectAll() {
    if (model != null)
      model.unselectAll();
  }

  function ensureIndexVisible(index) {
    grid.positionViewAtIndex(index, GridView.Visible);
  }

  // Uses black magic to hunt for the delegate instance with the given index.
  // Returns undefined if there's no currently-instantiated delegate with that
  // index.
  function getDelegateInstanceAt(index) {
    for(var i = 0; i < grid.contentItem.children.length; ++i) {
      var item = grid.contentItem.children[i];
      // We have to check for the specific objectName we gave our delegates
      // below, since we also get some items that were not our delegates here.
      if (item.objectName == "delegateItem" && item.index == index)
        return item;
    }
    return undefined;
  }

  // Uses getDelegateInstanceAt() to get the delegate instance with the given
  // index, then returns a rect with its coords relative to the given object.
  function getRectOfItemAt(index, relativeTo) {
    var item = getDelegateInstanceAt(index);
    if (!item)
      return undefined;

    var rect = mapToItem(relativeTo, item.x, item.y - grid.contentY);
    rect.width = item.width;
    rect.height = item.height;

    // Now we have to adjust for the border inside the delegate.
    var borderWidth = widthSansStroke - widthWithStroke;
    var borderHeight = heightSansStroke - heightWithStroke;
    rect.x += borderWidth / 2;
    rect.y += borderHeight / 2;
    rect.width -= borderWidth;
    rect.height -= borderHeight;

    return rect;
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
      // This name is checked for in getDelegateInstanceAt() above.
      objectName: "delegateItem"

      // This is necessary to expose the index property externally, like for
      // getDelegateInstanceAt() above.
      property int index: model.index

      Rectangle {
        width: widthSansStroke
        height: heightSansStroke
        
        Loader {
          id: loader
          
          property variant modelData: model
          
          anchors.centerIn: parent
          
          width: widthWithStroke
          height: heightWithStroke
          
          sourceComponent: checkerboard.delegate
        }
        
        MouseArea {
          anchors.fill: loader
          
          onPressAndHold: {
            // Press-and-hold activates selection mode,
            // but need to differentiate in onReleased whether it's a mode
            // change or a selection/activation
            if (allowSelection && checkerboard.state == "normal") {
              checkerboard.state = "to-selecting";
              checkerboard.model.toggleSelection(object);
            }
          }
          
          onReleased: {
            // See onPressAndHold for note on logic behind state changes
            if (checkerboard.state == "normal") {
              var rect = mapToItem(tablet_surface, parent.x, parent.y);
              rect.width = width;
              rect.height = height;
              checkerboard.activated(object, model, rect);
            } else if (allowSelection && checkerboard.state == "to-selecting") {
              checkerboard.state = "selecting";
            } else if (allowSelection && checkerboard.state == "selecting") {
              checkerboard.model.toggleSelection(object);
            }
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

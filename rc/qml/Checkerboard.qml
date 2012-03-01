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
import "GalleryUtility.js" as GalleryUtility

Rectangle {
  id: checkerboard
  
  signal activated(variant object, variant objectModel, variant activatedRect)
  
  signal movementStarted()
  signal movementEnded()
  
  property alias model: grid.model
  property Component delegate
  
  property alias contentX: grid.contentX
  property alias contentY: grid.contentY
  
  property int xMax: width / widthSansStroke
  
  property int widthSansStroke: gu(25.75)
  property int heightSansStroke: gu(19.5)
  property int widthWithStroke: gu(24.75)
  property int heightWithStroke: gu(18.5)
  
  property bool inSelectionMode: false
  property bool allowSelection: true
  property int selectedCount: (model) ? model.selectedCount : 0
  
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
  
  function getRectOfItem(item, relativeTo, adjustForBorder) {
    var rect = GalleryUtility.getRectRelativeTo(item, relativeTo);
    
    if (adjustForBorder) {
      // Now we have to adjust for the border inside the delegate.
      var borderWidth = widthSansStroke - widthWithStroke;
      var borderHeight = heightSansStroke - heightWithStroke;
      rect.x += borderWidth / 2;
      rect.y += borderHeight / 2;
      rect.width -= borderWidth;
      rect.height -= borderHeight;
    }
    
    return rect;
  }
  
  // Uses getDelegateInstanceAt() to get the delegate instance with the given
  // index, then returns a rect with its coords relative to the given object.
  function getRectOfItemAt(index, relativeTo) {
    var item = getDelegateInstanceAt(index);
    
    return (item) ? getRectOfItem(item, relativeTo, true) : undefined;
  }
  
  function getVisibleDelegates() {
    var vi = GalleryUtility.getVisibleItems(grid, grid, function(child) {
      return child.objectName == "delegateItem"
    });
    
    // return the delegates provided by host, not internal delegate
    var vd = [];
    for (var ctr = 0; ctr < vi.length; ctr++)
      vd[vd.length] = vi[ctr].item;
    
    return vd;
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
    
    onMovementStarted: checkerboard.movementStarted()
    onMovementEnded: checkerboard.movementEnded()
    
    delegate: Row {
      // This name is checked for in getDelegateInstanceAt() above.
      objectName: "delegateItem"

      // This is necessary to expose the index property externally, like for
      // getDelegateInstanceAt() above.
      property int index: model.index
      property Item item: loader.item
      
      Rectangle {
        width: widthSansStroke
        height: heightSansStroke
        
        Loader {
          id: loader
          
          property variant modelData: model
          
          anchors.centerIn: parent
          
          width: (checkerboard.state == "selecting" ||
            checkerboard.state == "to-selecting") ? widthSansStroke :
            widthWithStroke
          height: (checkerboard.state == "selecting" ||
            checkerboard.state == "to-selecting") ? heightSansStroke :
            heightWithStroke
          
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
              var rect = mapToItem(application, parent.x, parent.y);
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
          z: 8
          
          visible: isSelected
          
          asynchronous: true
          cache: true
          smooth: true
        }

        BorderImage {
          source: "../img/selected-border-stroke.png"

          anchors.fill: loader
          z: 4

          asynchronous: true
          cache: true

          visible: (checkerboard.state == "selecting" ||
            checkerboard.state == "to-selecting")
        }
      }
    }
  }
}

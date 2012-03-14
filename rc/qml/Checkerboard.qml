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
  
  property real itemWidth: gu(24)
  property real itemHeight: gu(19)
  property real gutterSize: gu(2)
  
  property bool allowActivation: true
  property bool inSelectionMode: false
  property bool allowSelection: true
  property int selectedCount: (model) ? model.selectedCount : 0
  
  // readonly
  property real delegateWidth: itemWidth + gutterSize
  property real delegateHeight: itemHeight + gutterSize

  function unselectAll() {
    if (model != null)
      model.unselectAll();
  }

  function ensureIndexVisible(index, centered) {
    grid.positionViewAtIndex(index, centered ? GridView.Center : GridView.Visible);
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
  
  function getRectOfItem(item, relativeTo, adjustForGutter) {
    var rect = GalleryUtility.getRectRelativeTo(item, relativeTo);
    
    if (adjustForGutter) {
      // Now we have to adjust for the gutter inside the delegate.
      rect.x += gutterSize / 2;
      rect.y += gutterSize / 2;
      rect.width -= gutterSize;
      rect.height -= gutterSize;
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
    
    anchors.fill: parent
    
    cellWidth: delegateWidth
    cellHeight: delegateHeight
    
    onMovementStarted: checkerboard.movementStarted()
    onMovementEnded: checkerboard.movementEnded()
    
    delegate: Item {
      // This name is checked for in getDelegateInstanceAt() above.
      objectName: "delegateItem"

      // This is necessary to expose the index property externally, like for
      // getDelegateInstanceAt() above.
      property int index: model.index
      property Item item: loader.item
      
      width: delegateWidth
      height: delegateHeight

      Loader {
        id: loader

        property variant modelData: model

        width: itemWidth
        height: itemHeight
        anchors.centerIn: parent

        sourceComponent: checkerboard.delegate
        
        MouseArea {
          anchors.fill: parent

          enabled: (allowActivation || allowSelection)
          
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
            if (allowActivation && checkerboard.state == "normal") {
              var rect = GalleryUtility.getRectRelativeTo(loader, checkerboard);
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

          anchors.fill: parent
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

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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import "GalleryUtility.js" as GalleryUtility

Rectangle {
  id: checkerboard
  
  signal activated(variant object, variant objectModel, variant activatedRect)
  
  signal movementStarted()
  signal movementEnded()
  
  property alias model: grid.model
  property alias delegate: grid.delegate
  
  property alias contentX: grid.contentX
  property alias contentY: grid.contentY
  
  property real itemWidth: gu(24)
  property real itemHeight: gu(18)
  property real gutterWidth: gu(2)
  property real gutterHeight: gu(2)
  
  property bool allowActivation: true
  property bool inSelectionMode: false
  property bool allowSelection: true
  property int selectedCount: (model) ? model.selectedCount : 0
  
  // readonly
  property real delegateWidth: itemWidth + gutterWidth
  property real delegateHeight: itemHeight + gutterHeight
  
  function selectAll() {
    if (model)
      model.selectAll();
  }
  
  function unselectAll() {
    if (model)
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
      // We have to check for the specific objectName we gave our delegates in
      // CheckerboardDelegate, since we also get some items that were not our
      // delegates here.
      if (item.objectName == "checkerboardDelegate" && item.index == index)
        return item;
    }
    return undefined;
  }
  
  function getRectOfItem(item, relativeTo, adjustForGutter) {
    var rect = GalleryUtility.getRectRelativeTo(item, relativeTo);
    
    if (adjustForGutter) {
      // Now we have to adjust for the gutter inside the delegate.
      rect.x += gutterWidth / 2;
      rect.y += gutterHeight / 2;
      rect.width -= gutterWidth;
      rect.height -= gutterHeight;
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
    return GalleryUtility.getVisibleItems(grid, grid, function(child) {
      return child.objectName == "checkerboardDelegate"
    });
  }
  
  color: "white"
  clip: true
  
  GridView {
    id: grid
    objectName: "grid"
    
    anchors.fill: parent
    
    cellWidth: delegateWidth
    cellHeight: delegateHeight
    
    onMovementStarted: checkerboard.movementStarted()
    onMovementEnded: checkerboard.movementEnded()
  }
}

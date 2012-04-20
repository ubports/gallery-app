/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import "GalleryUtility.js" as GalleryUtility
import "Gallery.js" as Gallery

// A child delegate for a Checkerboard.  Tightly coupled to Checkerboard due to
// some required interactions (e.g. selection).  This is a separate component
// so you can define your own.
Item {
  id: checkerboardDelegate
  // This name is checked for in Checkerboard.getDelegateInstanceAt().  Please
  // don't modify it.
  objectName: "checkerboardDelegate"

  signal swipeStarted(bool leftToRight, int start)
  signal swiping(bool leftToRight, int start, int distance)
  signal swiped(bool leftToRight)

  property variant checkerboard // The owning Checkerboard.
  // This delegate encompasses the content + the gutter.  content is the item
  // (sized checkerboard.itemWidth/Height) inside the gutter.  It will be
  // reparented internally so it's positioned correctly.
  property Item content
  // Whether to apply the default selection highlighting internally.
  property bool useInternalSelectionHighlight: true
  // Whether to use the default selection/activation handling internally.
  property alias handleInput: inputArea.enabled

  // readonly
  property variant modelData: model
  // Also needed for Checkerboard.getDelegateInstanceAt().
  property int index: modelData.index
  property bool isSelected: modelData.isSelected

  width: checkerboard.delegateWidth
  height: checkerboard.delegateHeight
  
  opacity: {
    if (!checkerboard.inSelectionMode)
      return 1.0;
    else if (useInternalSelectionHighlight && isSelected)
      return Gallery.SELECTED_OPACITY;
    else
      return Gallery.UNSELECTED_OPACITY;
  }
  
  Component.onCompleted: {
    if (content)
      content.parent = contentArea;
  }

  Item {
    id: contentArea

    width: checkerboard.itemWidth
    height: checkerboard.itemHeight
    anchors.centerIn: parent

    SwipeArea {
      id: inputArea

      anchors.fill: parent

      onLongPressed: {
        if (checkerboard.allowSelection && !checkerboard.inSelectionMode) {
          checkerboard.inSelectionMode = true;
          checkerboard.model.toggleSelection(modelData.object);
        }
      }

      onTapped: {
        if (checkerboard.inSelectionMode) {
          checkerboard.model.toggleSelection(modelData.object);
        } else {
          var rect = GalleryUtility.getRectRelativeTo(checkerboardDelegate, checkerboard);
          checkerboard.activated(modelData.object, modelData, rect);
        }
      }

      onStartSwipe: checkerboardDelegate.swipeStarted(leftToRight, start)
      onSwiping: checkerboardDelegate.swiping(leftToRight, start, distance)
      onSwiped: checkerboardDelegate.swiped(leftToRight)
    }
  }
}

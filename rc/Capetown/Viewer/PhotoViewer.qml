/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import ".."

// Displays a flickable photo stream.
//
// When implementing this, override onCurrentIndexChanged to load the 
// appropriate photo for the index.
Pager {
  id: photoViewer
  objectName: "photoViewer"
  
  property int currentIndexForHighlight: -1
  
  // NOTE: These properties should be treated as read-only, as setting them
  // individually can lead to bogus results.  Use setCurrentIndex() to 
  // initialize the view.
  property alias index: photoViewer.currentIndex
  
  function setCurrentIndex(index) {
    photoViewer.pageTo(index);
  }
  
  onMovingChanged: {
    // TODO: if you scroll through a number of pages without stopping, this
    // never gets updated, so the highlighting stops working.
    if (moving)
      currentIndexForHighlight = currentIndex;
  }
  
  // Keyboard focus while visible
  onVisibleChanged: {
    if (visible)
      forceActiveFocus();
  }
  
  Keys.onPressed: {
    if (event.key === Qt.Key_Left) {
      pageBack();
      event.accepted = true;
    } else if (event.key === Qt.Key_Right) {
      pageForward();
      event.accepted = true;
    }
  }
  
  // Background color fill.
  Rectangle {
    anchors.fill: parent
    
    z: -1000 //background
    color: "black"
  }
}

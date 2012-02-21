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
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 1.1

// Because some QML components' natural swiping motion requires custom
// event handling, this emulates it and allows for programmatic control
// when a swipe has occurred.
//
// TODO: Currently no support for vertical swiping
MouseArea {
  signal startSwipe(bool leftToRight, int state)
  signal swiping(bool leftToRight, int start, int distance)
  signal swiped(bool leftToRight)
  signal tapped()
  
  property bool enabled: true
  property int requiredHorizMovement: gu(0)
  
  // internal
  property int startX: -1
  property bool leftToRight: true
  property bool swipeStarted: false
  
  onEnabledChanged: {
    startX = -1;
    leftToRight = true;
    swipeStarted = false;
  }
  
  onPositionChanged: {
    if (!enabled)
      return;
    
    // look for initial swipe
    if (startX == -1) {
      startX = mouse.x;
      
      return;
    }
    
    var diff = 0;
    if (mouse.x < startX) {
      // once started, don't signal anything on other side of bounding point
      if (swipeStarted && leftToRight)
        return;
      
      diff = startX - mouse.x;
      leftToRight = false;
    } else if (mouse.x > startX) {
      // once started, don't signal anything on other side of bounding point
      if (swipeStarted && !leftToRight)
        return;
      
      diff = mouse.x - startX;
      leftToRight = true;
    }
    
    if (swipeStarted) {
      swiping(leftToRight, startX, diff);
      
      return;
    }
    
    if (diff < requiredHorizMovement)
      return;
    
    swipeStarted = true;
    
    startSwipe(leftToRight, startX);
    swiping(leftToRight, startX, diff);
  }
  
  onReleased: {
    if (enabled) {
      if (swipeStarted)
        swiped(leftToRight);
      else
        tapped();
    }
    
    startX = -1;
    leftToRight = true;
    swipeStarted = false;
  }
}

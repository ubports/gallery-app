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
// TODO: Add support for granular signalling of motion
MouseArea {
  signal swiped(bool leftToRight)
  
  signal tapped();
  
  property bool active: true
  
  // read-only
  property int requiredHorizMovement: 20
  
  // private
  property int startX: -1
  property bool wasSwipe: false
  
  onActiveChanged: {
    startX = -1;
  }
  
  onPositionChanged: {
    if (!active)
      return;
    
    // look for initial swipe
    if (startX == -1) {
      startX = mouse.x;
      
      return;
    }
    
    var diff = 0;
    var leftToRight = true;
    if (mouse.x < startX) {
      diff = startX - mouse.x;
      leftToRight = true;
    } else if (mouse.x > startX) {
      diff = mouse.x - startX;
      leftToRight = false;
    }
    
    if (diff < requiredHorizMovement)
      return;
    
    // reset start position and signal
    startX = -1;
    wasSwipe = true;
    
    swiped(leftToRight);
  }
  
  onReleased: {
    if (!wasSwipe)
      tapped();
    
    startX = -1;
    wasSwipe = false;
  }
}

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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0

// An Item that animates back and forth between two positions, normal ("out",
// given in the normal QML Item properties) and an alternate ("in", given in
// inX and inY).
Item {
  id: slidingPane

  property real inX
  property real inY
  property int duration: 200

  // readonly
  property bool isIn: (state == "in")
  property bool animationRunning: slideX.running || slideY.running

  function slideIn() {
    state = "in";
  }

  function slideOut() {
    state = "";
  }

  states: [
    State {
      name: "in"
      PropertyChanges { target: slidingPane; x: inX; y: inY; }
    }
  ]

  Behavior on x {
    animation: NumberAnimation {
      id: slideX
      easing.type: Easing.OutQuad
      duration: slidingPane.duration
    }
  }
  Behavior on y {
    animation: NumberAnimation {
      id: slideY
      easing.type: Easing.OutQuad
      duration: slidingPane.duration
    }
  }
}

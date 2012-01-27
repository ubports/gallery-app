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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1

// Sets the target to visible at the startOpacity, animates to the endOpacity.
SequentialAnimation {
  id: fadeInAnimation
  objectName: "fadeInAnimation"

  property variant target

  property real startOpacity: 0
  property real endOpacity: 1
  property real z: target.z

  property int duration: 200

  PropertyAction {
    target: fadeInAnimation.target
    property: "opacity"
    value: startOpacity
  }

  PropertyAction {
    target: fadeInAnimation.target
    property: "z"
    value: z
  }

  PropertyAction {
    target: fadeInAnimation.target
    property: "visible"
    value: true
  }

  NumberAnimation {
    target: fadeInAnimation.target
    property: "opacity"
    to: endOpacity
    duration: fadeInAnimation.duration
    easing.type: Easing.InQuad
  }
}

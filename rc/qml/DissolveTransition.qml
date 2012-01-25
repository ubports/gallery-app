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

Transition {
  property variant fadeInTarget;
  property variant fadeOutTarget;

  SequentialAnimation {
    PropertyAction {
      target: fadeInTarget
      property: "opacity"
      value: 0
    }

    PropertyAction {
      target: fadeInTarget
      property: "visible"
      value: true
    }

    ParallelAnimation {
      NumberAnimation {
        target: fadeInTarget
        property: "opacity"
        to: 1
        duration: 200
        easing.type: Easing.InQuad
      }

      NumberAnimation {
        target: fadeOutTarget
        property: "opacity"
        to: 0
        duration: 200
        easing.type: Easing.InQuad
      }
    }

    PropertyAction {
      target: fadeOutTarget
      property: "visible"
      value: false
    }
  }
}

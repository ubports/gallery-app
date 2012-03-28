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
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1

Item {
  id: toolbarIconButton

  property url selectedBackgroundFilename
  property url deselectedBackgroundFilename
  property url selectedIconFilename
  property url deselectedIconFilename

  signal pressed()
  signal pressedAndHeld()

  width: (background.source != "" ? background.width : gu(6))
  height: (background.source != "" ? background.height : gu(4))

  state: "deselected"
  states: [
    State { name: "selected"; },
    State { name: "deselected"; }
  ]

  Image {
    id: background

    source: (toolbarIconButton.state == "selected" ? selectedBackgroundFilename : deselectedBackgroundFilename)
  }

  Image {
    id: icon

    anchors.centerIn: parent
    source: (toolbarIconButton.state == "selected" ? selectedIconFilename : deselectedIconFilename)
  }

  MouseArea {
    anchors.fill: parent

    onPressed: toolbarIconButton.state = "selected"
    onReleased: toolbarIconButton.state = "deselected"
    onClicked: toolbarIconButton.pressed()
    onPressAndHold: toolbarIconButton.pressedAndHeld()
  }
}

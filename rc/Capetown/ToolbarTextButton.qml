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

Item {
  id: toolbarTextButton

  signal pressed()
  signal pressedAndHeld()

  property alias text: text.text

  // readonly
  property bool isSelected

  width: parts.width
  height: gu(4)

  Row {
    id: parts

    anchors.centerIn: parent
    width: childrenRect.width
    height: parent.height

    Image {
      source: (isSelected ? "img/button-left-active.png" : "img/button-left-inactive.png")
      cache: true
    }

    Image {
      source: (isSelected ? "img/button-body-active.png" : "img/button-body-inactive.png")
      cache: true
      fillMode: Image.TileHorizontally
      width: text.width

      Text {
        id: text

        anchors.verticalCenter: parent.verticalCenter

        color: (isSelected ? "white" : "#747273")
        font.pointSize: pointUnits(9)
        font.italic: true
        font.weight: Font.DemiBold
      }
    }

    Image {
      source: (isSelected ? "img/button-right-active.png" : "img/button-right-inactive.png")
      cache: true
    }
  }

  MouseArea {
    anchors.fill: parts

    onPressed: isSelected = true
    onReleased: isSelected = false
    onClicked: toolbarTextButton.pressed()
    onPressAndHold: toolbarTextButton.pressedAndHeld()
  }
}

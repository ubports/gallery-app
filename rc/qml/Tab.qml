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
 */

import QtQuick 1.1

Rectangle {
  id: tab
  
  property alias title: title.text
  property int fontPixelSize: 19

  property int deselectedBottomBorderWidth: 2
  property color deselectedBottomBorderColor: "#7da7d9"
  
  width: 110
  height: 44
  radius: 3

  border.color: "#7da7d9"
  clip: true
  
  signal activated()
  
  states: [
    State { name: "selected";
      PropertyChanges { target: tab; z: 10; }
      PropertyChanges { target: tab; color: "white"; }
      PropertyChanges { target: tab; border.width: 2; }
    },

    State { name: "deselected";
      PropertyChanges { target: tab; z: 0; }
      PropertyChanges { target: tab; color: "#c7d7ee"; }
      PropertyChanges { target: tab; border.width: 0; }
    }
  ]
  
  state: "deselected"
  
  // This guy squares off the bottom corners of the tab and covers the bottom border.
  Rectangle {
    z: parent.z
    width: parent.width - parent.border.width
    height: parent.border.width + parent.radius * 2
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.bottom
    color: parent.color
  }

  // And this guy draws a potentially different border up from the bottom if we're deselected.
  Rectangle {
    z: parent.z
    width: parent.width
    height: deselectedBottomBorderWidth
    anchors.bottom: parent.bottom
    color: deselectedBottomBorderColor
    visible: parent.state != "selected"
  }

  Text {
    id: title

    anchors.centerIn: parent
    color: "#659ad2"
    font.pixelSize: fontPixelSize
  }

  MouseArea {
    anchors.fill: parent

    onClicked: {
      if (state != "selected")
        activated();
    }
  }
}

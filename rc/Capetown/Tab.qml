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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1

Item {
  id: tab
  
  property alias title: title.text
  property int fontPointSize: pointUnits(9) // From the reference wireframes.

  property url selectedBackgroundSource
  property url deselectedBackgroundSource

  property color selectedTextColor: "white"
  property color deselectedTextColor: "#747273"

  signal activated()
  
  width: (background.source != "" ? background.width : gu(13.75))
  height: (background.source != "" ? background.height : gu(5.5))

  state: "deselected"
  states: [
    State { name: "selected"; },
    State { name: "deselected"; }
  ]
  
  Image {
    id: background

    source: (tab.state == "selected" ? selectedBackgroundSource : deselectedBackgroundSource)
    cache: true
  }

  Text {
    id: title

    anchors.centerIn: parent
    color: (tab.state == "selected" ? selectedTextColor : deselectedTextColor)
    font.pointSize: fontPointSize
    font.italic: true
    font.weight: (tab.state == "selected" ? Font.DemiBold : Font.Normal)
  }

  MouseArea {
    anchors.fill: parent

    onClicked: {
      if (state != "selected")
        activated();
    }
  }
}

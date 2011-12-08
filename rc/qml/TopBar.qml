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

import QtQuick 1.0

Rectangle {
  x: 0
  y: 0
  width: 1280
  height: 24
  color: "black"
  
  Image {
    id: power_icon
    objectName: "power_icon"

    source: "../img/power.png"
    x: parent.width - width - 2
    y: 2
  }
  
  MouseArea {
    onClicked: tablet_surface.power_off()
  
    x: power_icon.x
    y: power_icon.y
    width: power_icon.width
    height: power_icon.height
  }
}


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
  id: return_button
  objectName: "return_button"
  
  property string title: "Return"
  
  signal pressed()
  
  width: 192
  height: 48
  
  color: "transparent"
  
  Image {
    id: return_cap_icon
    objectName: "return_cap_icon"
    
    width: 18
    height: 48
    x: 0
    y: 0
    z: 0
    
    source: "return-cap.png"
  }
  
  Image {
    width: return_button.width - return_cap_icon.width
    height: 48
    x: 18
    y: 0
    z: 0
    
    source: "element-body.png"
    
    fillMode: Image.Tile
  }
  
  Text {
    anchors.centerIn: parent
    text: return_button.title
    color: "white"
    
    z: 10
  }
  
  MouseArea {
    anchors.fill: parent
    z: 20
    
    onClicked: return_button.pressed()
  }
}


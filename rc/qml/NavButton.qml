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
  id: nav_button
  objectName: "nav_button"
  
  signal pressed()
  
  property string title: "button"
  
  anchors.top: parent.top
  anchors.bottom: parent.bottom
  anchors.margins: 4
  
  width: 80
  height: parent.height - 8
  
  color: "white"
  radius: 10
  
  Text {
    text: parent.title
    
    anchors.fill: parent
    
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
  }
  
  MouseArea {
    anchors.fill: parent
    
    onClicked: parent.pressed()
  }
}

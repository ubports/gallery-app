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
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  id: eventCard
  
  property variant event
  property string textColor: "#95b5de"
  property string backgroundColor: "white"
  property bool hasBottomSeparator: false
  
  color: backgroundColor
  
  Text {
    id: text
    
    color: textColor
    
    anchors.fill: parent
    anchors.bottomMargin: 1.0
    
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    
    font.family: "Ubuntu"
    font.weight: Font.Bold
    font.pixelSize: gu(3)
    
    text: (event) ? Qt.formatDate(event.date, "d - M - yyyy") : ""
  }
  
  Rectangle {
    anchors.top: text.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    
    width: text.width - (parent.anchors.leftMargin + parent.anchors.rightMargin)
    height: hasBottomSeparator ? 1 : 0
    
    color: textColor
  }
}

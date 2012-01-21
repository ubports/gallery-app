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
  property string textColor: "mediumBlue"
  property string backgroundColor: "white"
  
  color: backgroundColor
  
  Text {
    color: textColor
    
    anchors.fill: parent
    
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    
    text: (event) ? Qt.formatDate(event.date, "d-M-yyyy") : ""
  }
}

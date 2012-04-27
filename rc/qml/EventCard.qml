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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

Item {
  id: eventCard
  
  property variant event
  property int mediaCount: -1
  property color textColor: "#6e6046"
  
  // internal
  property real cardWidth: gu(24)
  property real cardHeight: gu(18)
  // Where the transparent shadow ends in the card image.
  property real cardStartX: 2
  property real cardStartY: 2
  // marks in corners of the card
  property real cornersWidth: 14
  property real cornersHeight: 14
  
  // TODO: Warning: internationalization issues ahead
  function photosLabel(count) {
    return count + ((count == 1) ? " photo" : " photos");
  }
  
  Image {
    x: -cardStartX
    y: -cardStartY

    source: "../img/event-card.png"
    cache: true
    
    Item {
      // funky geometry produces an Item centered inside the card proper, not
      // the card plus shadow and border, with some space on top and bottom
      // to avoid the triangles in the corners
      x: parent.x + (cardStartX * 3) + cornersWidth
      y: parent.y + (cardStartY * 3) + cornersHeight
      width: cardWidth - (cardStartX * 2) - (cornersWidth * 2)
      height: cardHeight - (cardStartY * 2) - (cornersHeight * 2)
      
      Text {
        anchors.top: parent.top
        
        width: parent.width
        
        font.family: "Ubuntu"
        font.pointSize: pointUnits(9)
        color: textColor

        font.capitalization: Font.AllUppercase
        horizontalAlignment: Text.AlignHCenter

        text: (event) ? Qt.formatDate(event.date, "MMMM yyyy") : ""
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        
        width: parent.width

        font.family: "Ubuntu"
        font.pointSize: pointUnits(26)
        color: textColor

        horizontalAlignment: Text.AlignHCenter

        text: (event) ? Qt.formatDate(event.date, "dd") : ""
      }
      
      Text {
        anchors.bottom: parent.bottom
        
        width: parent.width
        
        font.family: "Ubuntu"
        font.pointSize: pointUnits(7)
        color: textColor
        
        font.capitalization: Font.AllUppercase
        horizontalAlignment: Text.AlignHCenter
        
        text: {
          if (mediaCount >= 0)
            return photosLabel(mediaCount)
          else if (event)
            return photosLabel(event.containedCount);
          else
            return "";
        }
      }
    }
  }
}

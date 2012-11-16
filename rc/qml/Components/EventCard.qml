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

import QtQuick 2.0
import Ubuntu.Components 0.1
import "../../js/Gallery.js" as Gallery

Item {
  id: eventCard
  
  property variant event
  property color textColor: "#919191"
  property color cardColor: "#dddddd"
  property bool isSelected
  
  // internal
  property real cardWidth: gu(18)
  property real cardHeight: gu(18)
  property real cardSafeAreaWidth: gu(15)
  property real cardSafeAreaHeight: gu(15)
  
  // TODO: Warning: internationalization issues ahead
  function photosLabel(count) {
    return count + ((count == 1) ? " photo" : " photos");
  }
  
  width: getDeviceSpecific("photoThumbnailWidth")
  height: getDeviceSpecific("photoThumbnailHeight")

  // TODO: we may want different graphical assets/font sizes here instead of
  // just scaling down the tablet-sized ones.
  transform: Scale {
    xScale: width / cardWidth
    yScale: height / cardHeight
  }

  // TODO: replace with the official rounded rectangle widget once
  // it lands in the SDK.
  Rectangle {
    id: eventCardRoundRect
    
    x: 0
    y: 0
    width: cardWidth
    height: cardHeight
    color: cardColor

    Item {
      x: parent.x + ((cardWidth - cardSafeAreaWidth) / 2.0)
      y: parent.y + ((cardHeight - cardSafeAreaHeight) / 2.0)
      width: cardSafeAreaWidth
      height: cardSafeAreaHeight
      
      TextCustom {
        id: eventMonthYear
        anchors.bottom: parent.bottom
        
        width: parent.width
        
        font.family: "Ubuntu"
        fontSize: "small"
        color: textColor

        font.capitalization: Font.AllUppercase
        horizontalAlignment: Text.AlignHCenter

        text: (event) ? Qt.formatDate(event.date, "MMM yyyy") : ""
      }

      TextCustom {
        id: eventDay
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        
        width: parent.width

        font.family: "Ubuntu"
        fontSize: "x-large"
        color: textColor

        horizontalAlignment: Text.AlignHCenter

        text: (event) ? Qt.formatDate(event.date, "dd") : ""
      }
    }
  }
}

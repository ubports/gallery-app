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
  property color textColor: "#6e6046"
  
  // internal
  property real cardWidth: gu(24)
  property real cardHeight: gu(18)
  // Where the transparent shadow ends in the card image.
  property real cardStartX: 2
  property real cardStartY: 2

  Image {
    x: -cardStartX
    y: -cardStartY

    source: "../img/event-card.png"
    cache: true

    Column {
      x: cardStartX
      y: cardStartY

      // Spacer
      Item {
        width: 1
        height: gu(2)
      }

      Text {
        width: cardWidth

        font.family: "Ubuntu"
        font.pointSize: pointUnits(9)
        color: textColor

        font.capitalization: Font.AllUppercase
        horizontalAlignment: Text.AlignHCenter

        text: (event) ? Qt.formatDate(event.date, "MMMM yyyy") : ""
      }

      Text {
        width: cardWidth

        font.family: "Ubuntu"
        font.pointSize: pointUnits(26)
        color: textColor

        horizontalAlignment: Text.AlignHCenter

        text: (event) ? Qt.formatDate(event.date, "dd") : ""
      }
    }
  }
}

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
 * Clint Rogers <clinton@yorba.org>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import "../../js/Gallery.js" as Gallery

UbuntuShape {
  id: eventCard

  property variant event
  property color textColor: "#919191"

  color: "#dddddd"
  radius: "medium"

  width: units.gu(18)
  height: units.gu(18)

  Column {
    anchors.centerIn: parent
    width: parent.width
 
    Label {
      id: eventDay

      width: parent.width

      font.family: "Ubuntu"
      fontSize: "x-large"
      color: textColor

      horizontalAlignment: Text.AlignHCenter

      text: (event) ? Qt.formatDate(event.date, "dd") : ""
    }

    Label {
      id: eventMonthYear
      width: parent.width

      font.family: "Ubuntu"
      fontSize: "small"
      color: textColor

      font.capitalization: Font.AllUppercase
      horizontalAlignment: Text.AlignHCenter

      text: (event) ? Qt.formatDate(event.date, "MMM yyyy") : ""
    }
  }
}

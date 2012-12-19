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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import "../../js/Gallery.js" as Gallery
import "../Utility"

//UbuntuShape {
RoundCornerShape {
  id: eventCard

  property variant event
  property color textColor: "#888888"

  radius: "medium"
  color: "#dddddd"
  opacity: 0.8

  width: units.gu(12)
  height: units.gu(12)

  Label {
    id: eventDay

    y: units.gu(0.5)
    width: parent.width

    font.family: "Ubuntu"

    font.pixelSize: units.dp(62)
    color: textColor

    horizontalAlignment: Text.AlignHCenter

    text: (event) ? Qt.formatDate(event.date, "dd") : ""
  }

  Label {
    id: eventMonthYear

    y: units.gu(8.5)
    width: parent.width

    font.family: "Ubuntu"
    fontSize: "medium"
    color: textColor

    font.capitalization: Font.AllUppercase
    horizontalAlignment: Text.AlignHCenter

    text: (event) ? Qt.formatDate(event.date, "MMM yyyy") : ""
  }
}

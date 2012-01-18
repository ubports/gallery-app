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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1

Rectangle {
  property bool isCompact: false
  property string title: ""
  property string textStyle: "light"

  signal pressed()

  width: (isCompact) ? 72 : 122
  height: (isCompact) ? 32 : 36

  color: "#a7a9ac"

  Text {
    anchors.centerIn: parent

    text: parent.title

    color: (parent.textStyle == "dark") ? "black" : "white"
  }

  MouseArea {
    anchors.fill: parent

    onClicked: parent.pressed()
  }
}

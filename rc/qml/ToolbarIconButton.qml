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
  id: wrapper
  objectName: "wrapper"

  property string iconFilename
  property bool isSquare: false

  signal pressed();

  /* spec'd normal icons are exactly 48px x 32px, spec'd square icons are
     exactly 48px x 48px*/
  width: 48
  height: (isSquare) ? 48 : 32

  color: "transparent"

  Image {
    z: 0
    anchors.fill: parent

    source: wrapper.iconFilename

    opacity: 1.0
  }

  MouseArea {
    z: 1
    anchors.fill: parent

    onClicked: wrapper.pressed()
  }
}

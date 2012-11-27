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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0

Item {
  property bool isTextured: true
  property bool isTranslucent: false
  property bool isDark: false
  property bool isBottom: false // vs. top; what part of the screen it's on.

  width: parent.width
  height: units.gu(6)

  opacity: (isTranslucent ? (isDark ? 0.7 : 0.9) : 1.0)

  Rectangle {
    id: darkBackground

    anchors.fill: parent

    color: "black"

    visible: !isTextured && isDark
  }

  Image {
    id: backgroundTexture

    anchors.fill: parent

    source: (isBottom
      ? "img/toolbar-background-bottom.png"
      : "img/toolbar-background-top.png")
    fillMode: Image.Tile

    visible: isTextured
  }

  Image {
    id: shadow

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: (isBottom ? undefined : parent.bottom)
    anchors.bottom: (isBottom ? parent.top : undefined)

    source: (isBottom
      ? "img/toolbar-shadow-above.png"
      : "img/toolbar-shadow-below.png")
    fillMode: Image.Tile

    visible: isTextured
  }
}

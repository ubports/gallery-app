/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
  id: popupBox

  property int contentLeft: 1
  property int contentTop: 1
  property int contentWidth: width - 2
  property int contentHeight: height - originCueImage.height - 2
  property int originCueHeight: originCueImage.height

  anchors.fill: parent

  color: "transparent"

  Rectangle {
    id: contentArea

    anchors.top: parent.top
    anchors.left: parent.left
    width: parent.width
    height: parent.height - originCueImage.height;
    z: 0

    color: "#bcbdc0"
    border.color: "#a7a9ac"
    border.width: 1
  }

  Image {
    id: originCueImage

    x: parent.width - width + 1
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 1
    width: 39
    height: 25
    z: 1

    source: "../img/popup-arrow.png"
  }
}

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
  property alias contentWidth: contentArea.width
  property alias contentHeight: contentArea.height
  
  property int originCueX: width - originCueWidth
  property alias originCueHeight: originCueImage.height
  property alias originCueWidth: originCueImage.width

  color: "transparent"

  Rectangle {
    id: contentArea
    
    anchors.top: parent.top
    anchors.left: parent.left
    width: parent.width
    height: parent.height - originCueHeight;
    
    color: "#bcbdc0"
    border.color: "#a7a9ac"
    border.width: 1
  }
  
  Image {
    id: originCueImage
    
    x: originCueX
    anchors.top: contentArea.bottom
    width: 39
    height: 25
    
    source: "../img/popup-arrow.png"
  }
}

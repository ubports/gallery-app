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
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 1.1

Item {
  id: albumPreviewA
  
  property variant mediaSourceList

  property real topGutter: (parent ? parent.topGutter : 0)
  property real bottomGutter: (parent ? parent.bottomGutter : 0)
  property real leftGutter: (parent ? parent.leftGutter : 0)
  property real rightGutter: (parent ? parent.rightGutter : 0)
  property real spineGutter: (parent ? parent.spineGutter : 0)

  property bool isPreview: false

  property real spineBorderWidth: (parent ? parent.spineBorderWidth : 0)
  property color spineBorderColor: (parent ? parent.spineBorderColor : "#95b5df")

  FramePortrait {
    id: left
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.horizontalCenter
    
    topGutter: albumPreviewA.topGutter
    bottomGutter: albumPreviewA.bottomGutter
    leftGutter: albumPreviewA.leftGutter
    rightGutter: albumPreviewA.spineGutter

    mediaSource: (mediaSourceList != null) ? mediaSourceList[0] : null
    isPreview: parent.isPreview
  }
  
  FramePortrait {
    id: right
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right
    
    topGutter: albumPreviewA.topGutter
    bottomGutter: albumPreviewA.bottomGutter
    leftGutter: albumPreviewA.spineGutter
    rightGutter: albumPreviewA.rightGutter
    
    mediaSource: (mediaSourceList != null) ? mediaSourceList[1] : null
    isPreview: parent.isPreview
  }

  Rectangle {
    id: spineBorder

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    width: spineBorderWidth

    anchors.topMargin: albumPreviewA.topGutter
    anchors.bottomMargin: albumPreviewA.bottomGutter

    color: spineBorderColor
  }
}

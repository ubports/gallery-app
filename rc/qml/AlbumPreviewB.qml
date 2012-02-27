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
  id: albumPreviewB
  
  property variant mediaSourceList

  property real topGutter: (parent ? parent.topGutter : 0)
  property real bottomGutter: (parent ? parent.bottomGutter : 0)
  property real leftGutter: (parent ? parent.leftGutter : 0)
  property real rightGutter: (parent ? parent.rightGutter : 0)
  property real spineGutter: (parent ? parent.spineGutter : 0)
  property real insideGutter: (parent ? parent.insideGutter : 0)

  property bool isPreview: false
  
  property real spineBorderWidth: (parent ? parent.spineBorderWidth : 0)
  property color spineBorderColor: (parent ? parent.spineBorderColor : "#95b5df")

  FramePortrait {
    id: top

    anchors.top: parent.top
    anchors.bottom: parent.verticalCenter
    anchors.left: parent.left
    anchors.right: parent.horizontalCenter

    topGutter: albumPreviewB.topGutter
    bottomGutter: albumPreviewB.insideGutter
    leftGutter: albumPreviewB.leftGutter
    rightGutter: albumPreviewB.spineGutter

    mediaSource: (mediaSourceList != null) ? mediaSourceList[0] : null
    isPreview: albumPreviewB.isPreview
  }

  FramePortrait {
    id: bottom

    anchors.top: parent.verticalCenter
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.horizontalCenter

    topGutter: albumPreviewB.insideGutter
    bottomGutter: albumPreviewB.bottomGutter
    leftGutter: albumPreviewB.leftGutter
    rightGutter: albumPreviewB.spineGutter

    mediaSource: (mediaSourceList != null) ? mediaSourceList[1] : null
    isPreview: albumPreviewB.isPreview
  }
  
  FramePortrait {
    id: right
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    topGutter: albumPreviewB.topGutter
    bottomGutter: albumPreviewB.bottomGutter
    leftGutter: albumPreviewB.spineGutter
    rightGutter: albumPreviewB.rightGutter
    
    mediaSource: (mediaSourceList != null) ? mediaSourceList[2] : null
    isPreview: albumPreviewB.isPreview
  }

  Rectangle {
    id: spineBorder

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    width: spineBorderWidth

    anchors.topMargin: albumPreviewB.topGutter
    anchors.bottomMargin: albumPreviewB.bottomGutter

    color: spineBorderColor
  }
}

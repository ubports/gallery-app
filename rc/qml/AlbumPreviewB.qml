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

Row {
  id: albumPreviewB
  
  property variant mediaSourceList
  property int gutter: (parent ? parent.gutter : gu(3))
  property int photoBorderWidth: (parent ? parent.photoBorderWidth : gu(0))
  property color photoBorderColor: (parent ? parent.photoBorderColor : "#95b5de")
  property bool isPreview: false
  
  Column {
    id: left
    
    width: parent.width / 2
    height: parent.height
    
    FramePortrait {
      id: top
      
      width: parent.width
      height: (parent.height / 2) - (horizontalGutter.height / 2)
      
      photoBorderWidth: albumPreviewB.photoBorderWidth
      photoBorderColor: albumPreviewB.photoBorderColor
      
      topGutter: gu(0)
      bottomGutter: gu(0)
      leftGutter: gu(3)
      rightGutter: gu(3)
      
      mediaSource: (mediaSourceList != null) ? mediaSourceList[0] : null
      isPreview: albumPreviewB.isPreview
    }
    
    // Shared gutter between two pictures
    Rectangle {
      id: horizontalGutter
      
      width: parent.width
      height: gu(3)
    }
    
    FramePortrait {
      id: bottom
      
      width: parent.width
      height: (parent.height / 2) - (horizontalGutter.height / 2)
      
      photoBorderWidth: albumPreviewB.photoBorderWidth
      photoBorderColor: albumPreviewB.photoBorderColor
      
      topGutter: gu(0)
      bottomGutter: gu(0)
      leftGutter: gu(3)
      rightGutter: gu(3)
      
      mediaSource: (mediaSourceList != null) ? mediaSourceList[1] : null
      isPreview: albumPreviewB.isPreview
    }
  }
  
  FramePortrait {
    id: right
    
    width: parent.width / 2
    height: parent.height
    
    photoBorderWidth: albumPreviewB.photoBorderWidth
    photoBorderColor: albumPreviewB.photoBorderColor
    
    topGutter: gu(0)
    bottomGutter: gu(0)
    leftGutter: gu(3)
    rightGutter: gu(3)
    
    mediaSource: (mediaSourceList != null) ? mediaSourceList[2] : null
    isPreview: albumPreviewB.isPreview
  }
}

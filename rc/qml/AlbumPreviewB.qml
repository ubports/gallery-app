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
  property int gutter: (parent ? parent.gutter : 24)
  property bool isPreview: false
  
  Column {
    id: left
    
    width: parent.width / 2
    height: parent.height
    
    FramePortrait {
      id: top
      
      width: parent.width
      height: parent.height / 2
      
      mediaSource: (mediaSourceList != null) ? mediaSourceList[0] : null
      gutter: albumPreviewB.gutter
      isPreview: albumPreviewB.isPreview
    }
    
    FramePortrait {
      id: bottom
      
      width: parent.width
      height: parent.height / 2
      
      mediaSource: (mediaSourceList != null) ? mediaSourceList[1] : null
      gutter: albumPreviewB.gutter
      isPreview: albumPreviewB.isPreview
    }
  }
  
  FramePortrait {
    id: right
    
    width: parent.width / 2
    height: parent.height
    
    mediaSource: (mediaSourceList != null) ? mediaSourceList[2] : null
    gutter: albumPreviewB.gutter
    isPreview: albumPreviewB.isPreview
  }
}

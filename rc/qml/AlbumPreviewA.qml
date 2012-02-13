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
  id: albumPreviewA
  
  property variant mediaSourceList
  property int gutter: (parent ? parent.gutter : 24)
  property int photoBorderWidth: (parent ? parent.photoBorderWidth : 0)
  property color photoBorderColor: (parent ? parent.photoBorderColor : "#95b5de")
  property bool isPreview: false
  
  FramePortrait {
    id: left
    
    width: (parent.width / 2)
    height: parent.height
    
    photoBorderWidth: albumPreviewA.photoBorderWidth
    photoBorderColor: albumPreviewA.photoBorderColor

    mediaSource: (mediaSourceList != null) ? mediaSourceList[0] : null
    gutter: parent.gutter
    isPreview: parent.isPreview
  }
  
  FramePortrait {
    id: right
    
    width: (parent.width / 2)
    height: parent.height
    
    photoBorderWidth: albumPreviewA.photoBorderWidth
    photoBorderColor: albumPreviewA.photoBorderColor

    mediaSource: (mediaSourceList != null) ? mediaSourceList[1] : null
    gutter: parent.gutter
    isPreview: parent.isPreview
  }
}

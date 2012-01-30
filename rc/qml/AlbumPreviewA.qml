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
  objectName: "albumPreviewA"
  
  property variant mediaSourceList
  property int gutter: 24
  property int borderWidth: 3
  
  FramePortrait {
    id: left
    objectName: "left"
    
    width: (parent.width / 2) - borderWidth
    height: parent.height - borderWidth
    
    mediaSource: (mediaSourceList != null) ? mediaSourceList[0] : null
    gutter: parent.gutter
    borderWidth: parent.borderWidth
  }
  
  Rectangle {
    id: divider
    objectName: "divider"
    
    width: left.borderWidth
    height: parent.height
    color: "#657CA9"
  }
  
  FramePortrait {
    id: right
    objectName: "right"
    
    width: (parent.width / 2) - borderWidth
    height: parent.height - borderWidth
    
    mediaSource: (mediaSourceList != null) ? mediaSourceList[1] : null
    gutter: parent.gutter
    borderWidth: parent.borderWidth
  }
}

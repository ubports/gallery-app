/*
 * Copyright (C) 2012 Canonical Ltd
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
import Gallery 1.0

Item {
  id: albumCover
  
  property Album album
  property bool anchorRight: true
  property bool isBlank: false
  property real titleOpacity: 1
  property int titleDateSpacing: gu(2)
  
  Rectangle {
    id: cover
    
    anchors.left: (anchorRight) ? parent.horizontalCenter : undefined
    anchors.right: (anchorRight) ? undefined : parent.horizontalCenter
    
    // maintain an aspect ratio, no matter the size
    width: ((height * 0.75) <= (parent.width / 2)) ? height * 0.75 : parent.width / 2
    height: parent.height
    
    border.width: gu(0.25)
    border.color: "#657CA9"
    
    clip: true
    
    Rectangle {
      width: (parent.width > parent.height) ? parent.width : parent.height
      height: width
      
      gradient: Gradient {
        GradientStop { position: 0.0; color: "#95b5de"; }
        GradientStop { position: 1.0; color: "#657CA9"; }
      }
      
      rotation: -90.0
    }
    
    Rectangle {
      anchors.verticalCenter: parent.verticalCenter
      
      width: parent.width
      height: titleText.height + dateText.height + titleDateSpacing
      
      color: "transparent"
      
      Text {
        id: titleText
        
        anchors.top: parent.top
        
        width: parent.width
        
        visible: !isBlank
        opacity: titleOpacity
        color: "#657CA9"
        
        font.family: "Ubuntu"
        font.weight: Font.Bold
        font.pixelSize: gu(3)
        
        smooth: true
        
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        maximumLineCount: 3
        
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        
        text: (album) ? album.name : ""
      }
      
      Text {
        id: dateText
        
        anchors.top: titleText.bottom
        anchors.topMargin: titleDateSpacing
        
        width: parent.width
        
        visible: !isBlank
        opacity: titleOpacity
        color: "#657CA9"
        
        font.family: "Ubuntu"
        font.weight: Font.Normal
        font.pixelSize: gu(2.25)
        
        smooth: true
        
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        maximumLineCount: 1
        
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        
        text: (album) ? Qt.formatDateTime(album.creationDateTime, "MM/dd/yy") : ""
      }
    }
  }
}

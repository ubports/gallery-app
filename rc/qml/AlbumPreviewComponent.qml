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
import Gallery 1.0

Rectangle {
  id: albumPreviewComponent
  
  property Album album
  property alias pageGutter: albumPageComponent.gutter
  property alias bookmarkOpacity: bookmark.opacity
  property alias nameHeight: textContainer.height
  property int frameBorderWidth: gu(0.375)
  property color frameBorderColor: "#95b5de"
  property int photoBorderWidth: gu(0.5)
  property color photoBorderColor: "#95b5de"

  // Read-only, please.
  property real canonicalWidth: gu(48.5)
  
  width: canonicalWidth
  height: gu(31.5)
  
  AlbumCover {
    id: albumCover
    
    album: parent.album
    
    anchors.fill: parent
    
    visible: (album) ? album.isClosed : false
  }
  
  Rectangle {
    id: albumPreview
    
    anchors.fill: parent
    
    visible: (album) ? !album.isClosed : false
    
    AlbumPageComponent {
      id: albumPageComponent
      
      photoBorderWidth: albumPreviewComponent.photoBorderWidth
      photoBorderColor: albumPreviewComponent.photoBorderColor

      albumPage: (album && !album.isClosed) ? album.currentPage : null
      gutter: gu(1)
      isPreview: true
      
      width: parent.width
      height: parent.height - textContainer.height
    }
    
    Row {
      anchors.fill: albumPageComponent

      Rectangle {
        width: albumPageComponent.width / 2
        height: albumPageComponent.height
        color: "transparent"
        border.width: frameBorderWidth
        border.color: frameBorderColor
      }
      Rectangle {
        width: albumPageComponent.width / 2
        height: albumPageComponent.height
        color: "transparent"
        border.width: frameBorderWidth
        border.color: frameBorderColor
      }
    }

    Rectangle {
      id: textContainer

      height: gu(6)
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: albumPageComponent.bottom

      Text {
        id: text

        anchors.centerIn: parent

        color: "#657CA9"

        text: (album) ? album.name : "";
        font.pixelSize: gu(3)
        smooth: true
      }
    }

    Image {
      id: bookmark
      
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: albumPageComponent.top
      
      transformOrigin: Item.Top
      scale: parent.width / canonicalWidth // Scale the bookmark accordingly if the preview has been resized.
      
      source: "../img/bookmark-ribbon.png"
      cache: true
    }
  }
}

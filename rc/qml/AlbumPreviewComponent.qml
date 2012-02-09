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
  property alias nameHeight: text.height
  property int borderWidth: 3

  // Read-only, please.
  property real canonicalWidth: 388
  
  width: canonicalWidth
  height: 252
  
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
      
      albumPage: (album && !album.isClosed) ? album.currentPage : null
      gutter: 8
      isPreview: true
      
      width: parent.width
      height: parent.height - text.height
    }
    
    Row {
      anchors.fill: albumPageComponent

      Rectangle {
        width: albumPageComponent.width / 2
        height: albumPageComponent.height
        color: "#00000000"
        border.width: borderWidth
        border.color: "#95b5de"
      }
      Rectangle {
        width: albumPageComponent.width / 2
        height: albumPageComponent.height
        color: "#00000000"
        border.width: borderWidth
        border.color: "#95b5de"
      }
    }

    Text {
      id: text
      
      height: 24
      
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: albumPageComponent.bottom
      anchors.topMargin: 12
      anchors.bottomMargin: 12
      
      color: "#657CA9"
      
      text: (album) ? album.name : "";
      smooth: true
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

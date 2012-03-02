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
  property alias bookmarkOpacity: bookmark.opacity
  property alias nameHeight: textContainer.height

  property real borderWidth: gu(0.25)
  property color borderColor: "#0072bc"

  property alias topGutter: albumPageComponent.topGutter
  property alias bottomGutter: albumPageComponent.bottomGutter
  property alias leftGutter: albumPageComponent.leftGutter
  property alias rightGutter: albumPageComponent.rightGutter
  property alias spineGutter: albumPageComponent.spineGutter
  property alias insideGutter: albumPageComponent.insideGutter

  // Read-only, please.
  property real canonicalWidth: gu(50)
  
  width: canonicalWidth
  height: gu(40)
  
  AlbumCover {
    id: albumCover
    
    album: parent.album
    
    anchors.fill: parent
    
    visible: (album) ? album.closed : false
  }
  
  Rectangle {
    id: albumPreview
    
    anchors.fill: parent
    
    visible: (album) ? !album.closed : false
    
    AlbumPageComponent {
      id: albumPageComponent
      
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.bottom: textContainer.top
      anchors.topMargin: gu(1)
      anchors.bottomMargin: gu(1)

      border.width: borderWidth
      border.color: borderColor

      spineBorderWidth: borderWidth
      spineBorderColor: borderColor

      topGutter: gu(2)
      bottomGutter: gu(2)
      leftGutter: gu(1)
      rightGutter: gu(1)
      spineGutter: gu(1)
      insideGutter: gu(0.5)

      albumPage: (album ? album.currentPage : null)
      isPreview: true
    }
    
    Rectangle {
      id: textContainer

      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      height: gu(7)

      Column {
        anchors.centerIn: parent

        Text {
          anchors.horizontalCenter: parent.horizontalCenter

          text: (album) ? album.name : "";
          color: "#818285"
          font.pointSize: 18 // Straight from the spec.
          smooth: true
        }

        Item { // Vertical spacer.
          width: 1
          height: gu(1)
        }

        Text {
          anchors.horizontalCenter: parent.horizontalCenter

          // TODO: subtitle, not creation date.
          text: (album) ? Qt.formatDate(album.creationDate, "MM/dd/yy") : ""
          color: "#a7a9ac"
          font.pointSize: 14 // From the spec.
          smooth: true
        }
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

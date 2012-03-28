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
  
  property int backCoverExposedWidth: gu(0.625)
  property int backCoverSpineWidth: gu(0.125)
  
  // Read-only, please.
  property real openHorizontalMargin: gu(1)
  property real titleHeight: gu(7)
  
  width: gu(28)
  height: gu(33)
  
  AlbumCover {
    anchors.fill: parent
    
    album: albumPreviewComponent.album
    
    visible: (album) ? album.closed : false
  }
  
  Column {
    anchors.fill: parent
    
    visible: (album) ? !album.closed : false
    
    // Back cover is exposed on three edges, with a thinner line for the spine
    Rectangle {
      id: backCover
      
      // TODO: Should only have rounded corners on the right-hand corners of
      // the folded-back cover ... QML currently doesn't support rounding
      // Rectangle corners selectively
      color: "#5f5952"
      radius: 4.0
      
      width: albumPreviewComponent.width
      height: albumPreviewComponent.height
      
      AlbumPageComponent {
        id: albumPageComponent

        x: backCoverSpineWidth
        y: backCoverExposedWidth
        width: albumPreviewComponent.width - (backCoverSpineWidth + backCoverExposedWidth)
        height: albumPreviewComponent.height - (backCoverExposedWidth * 2)

        album: albumPreviewComponent.album
        frontPageNumber: (album ? album.currentPageNumber + 1 : -1)
        isPreview: true
      }
    }
    
    // Spacer
    Item {
      width: 1
      height: gu(1)
    }
    
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      
      text: (album) ? album.name : "";
      color: "#818285"
      font.family: "Ubuntu"
      font.pointSize: 14
      smooth: true
    }
    
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      
      // TODO: subtitle, not creation date.
      text: (album) ? Qt.formatDateTime(album.creationDateTime, "MM/dd/yy") : ""
      color: "#a7a9ac"
      font.family: "Ubuntu"
      font.pointSize: 9 // From the spec.
      font.bold: true
      smooth: true
    }
  }
}

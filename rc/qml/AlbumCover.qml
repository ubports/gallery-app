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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

Item {
  id: albumCover
  
  property Album album
  property bool isBack: false
  property bool isBlank: false
  property real titleOpacity: 1
  property int titleDateSpacing: gu(2)
  
  // internal
  property real canonicalWidth: gu(28)
  property real canonicalHeight: gu(33)
  // Where the transparent shadow ends in the cover image.
  property real coverStartX: 5
  property real coverStartY: 5

  Image {
    id: cover
    
    x: -coverStartX
    y: -coverStartY

    transform: Scale {
      origin.x: coverStartX
      origin.y: coverStartY
      xScale: parent.width / canonicalWidth
      yScale: parent.height / canonicalHeight
    }

    source: "../img/album-cover.png"
    cache: true
    mirror: isBack

    Column {
      x: coverStartX
      y: coverStartY

      visible: !isBack && !isBlank
      
      // Spacer
      Item {
        width: 1
        height: gu(6)
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        width: canonicalWidth

        opacity: titleOpacity
        color: "#f5e8e0"
        
        font.family: "Nimbus Roman No9 L"
        font.pointSize: pointUnits(16) // From the spec.
        smooth: true
        
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        maximumLineCount: 3
        horizontalAlignment: Text.AlignHCenter

        text: (album) ? album.name : ""
      }
      
      // Spacer
      Item {
        width: 1
        height: titleDateSpacing
      }

      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        width: canonicalWidth

        opacity: titleOpacity
        color: "#f5e8e0"
        
        font.family: "Nimbus Roman No9 L"
        font.pointSize: pointUnits(10) // From the spec.
        smooth: true
        
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        maximumLineCount: 1
        horizontalAlignment: Text.AlignHCenter

        text: (album) ? Qt.formatDateTime(album.creationDateTime, "MM/dd/yy") : ""
      }
    }
  }
}

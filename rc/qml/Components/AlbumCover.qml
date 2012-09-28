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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import "../../Capetown"
import "../Utility"

Item {
  id: albumCover
  
  signal pressed(variant mouse)
  
  property Album album
  property bool isBack: false
  property bool isBlank: false
  property bool isPreview: true
  property real titleOpacity: 1
  property int titleDateSpacing: gu(2) // (Preview-sized; will scale up)
  
  // internal
  property real canonicalPreviewWidth: gu(28)
  property real canonicalPreviewHeight: gu(33)
  property real canonicalFullWidth: gu(66)
  property real canonicalFullHeight: gu(80)
  property real canonicalWidth: (isPreview ? canonicalPreviewWidth : canonicalFullWidth)
  property real canonicalHeight: (isPreview ? canonicalPreviewHeight : canonicalFullHeight)
  property real textScale: canonicalWidth / canonicalPreviewWidth
  property real spacerScale: canonicalHeight / canonicalPreviewHeight
  // Where the transparent shadow ends in the cover image.
  property real coverStartX: 5
  property real coverStartY: 5
  property variant coverElement: album !== null ?
    coverList.elementForActionName(album.coverNickname) : coverList.getDefault();
  
  // Read-only
  property bool isTextEditing: title.isEditing || subtitle.isEditing
  
  // Stops editing title/subtitle.
  function editingDone() {
    title.done();
    subtitle.done();
  }
  
  Image {
    id: cover
    
    x: -coverStartX
    y: -coverStartY

    transform: Scale {
      origin.x: coverStartX
      origin.y: coverStartY
      xScale: albumCover.width / canonicalWidth
      yScale: albumCover.height / canonicalHeight
    }
    
    AlbumCoverList {
      id: coverList
    }
    
    source: isPreview ? coverElement.imagePreview : coverElement.imageFull;
    
    cache: true
    mirror: isBack
    
    // Must be positioned before TextEdit elements to capture mouse events
    // underneath the album title.
    MouseArea {
      anchors.fill: parent
      
      acceptedButtons: Qt.LeftButton | Qt.RightButton
      onPressed: {
        mouse.accepted = false;
        albumCover.pressed(mouse); 
      }
    }
    
    Column {
      x: coverStartX
      y: coverStartY

      visible: !isBack && !isBlank
      
      // Spacer
      Item {
        width: 1
        height: gu(6) * spacerScale
      }

      TextEditOnClick {
        id: title
        
        text: (album) ? album.title : ""
        onTextUpdated: album.title = text
        
        editable: !isPreview
        
        anchors.horizontalCenter: parent.horizontalCenter
        width: canonicalWidth
        
        opacity: titleOpacity
        color: "#f5e8e0"
        
        fontFamily: "Nimbus Roman No9 L"
        fontPointSize: pointUnits(16) * textScale // From the spec.
        smooth: true
        textFormat: TextEdit.PlainText
        
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        
        onEnterPressed: {
          // If the user hits enter, start editing the subtitle.
          done();
          subtitle.start(-1, -1);
        }
      }
      
      // Spacer
      Item {
        width: 1
        height: titleDateSpacing * spacerScale
      }
      
      TextEditOnClick {
        id: subtitle
        
        text: (album) ? album.subtitle : ""
        onTextUpdated: album.subtitle = text
        
        editable: !isPreview
        
        anchors.horizontalCenter: parent.horizontalCenter
        width: canonicalWidth
        
        opacity: titleOpacity
        color: "#f5e8e0"
        
        fontFamily: "Nimbus Roman No9 L"
        
        // The -1 is due to a slight mismatch in preview vs. full album
        // cover aspect ratios.
        fontPointSize: pointUnits(10) * textScale - 1
        smooth: true
        textFormat: TextEdit.PlainText
        
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
      }
    }
  }
}

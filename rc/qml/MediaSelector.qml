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

Rectangle {
  id: mediaSelector
  objectName: "mediaSelector"
  
  signal addSelected()
  signal finished()
  
  anchors.fill: parent
  
  NavToolbar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    NavButton {
      id: deselectButton
      objectName: "deselectButton"
      
      anchors.right: doneButton.left
      
      title: "deselect"
      
      onPressed: mediaCheckerboard.unselect_all()
    }
    
    NavButton {
      id: doneButton
      objectName: "doneButton"
      
      anchors.right: parent.right
      
      title: "done"
      
      onPressed: finished()
    }
  }
  
  Checkerboard {
    id: mediaCheckerboard
    objectName: "mediaCheckerboard"
    
    anchors.top: navbar.bottom
    anchors.left: navbar.left
    anchors.right: navbar.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    allow_selection: true
    state: "selecting"
    
    checkerboardModel: ctx_media_selector_model
    checkerboardDelegate: PhotoComponent {
      mediaSource: modelData.media_source
      isCropped: true
      isPreview: true
    }
  }
  
  NavToolbar {
    id: toolbar
    objectName: "toolbar"
    
    anchors.bottom: parent.bottom
    
    Text {
      anchors.centerIn: parent
      
      text: "Select photos or movieclip(s) to add"
      visible: mediaCheckerboard.selected_count == 0
    }
    
    NavButton {
      anchors.right: parent.right
      
      title: "add"
      visible: mediaCheckerboard.selected_count > 0
      
      onPressed: addSelected()
    }
  }
}

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
  id: mediaSelector
  objectName: "mediaSelector"
  
  property variant album
  
  anchors.fill: parent
  
  EventCheckerboard {
    id: mediaCheckerboard
    objectName: "mediaCheckerboard"
    
    anchors.fill: parent
    anchors.topMargin: chrome.navbarHeight + gu(3)
    anchors.leftMargin: gu(2)
    anchors.rightMargin: gu(2)
    
    allowSelection: true
    state: "selecting"
  }

  ViewerChrome {
    id: chrome

    anchors.fill: parent

    fadeDuration: 0
    autoHideWait: 0

    navbarSelectionDoneButtonTitle: "add to album"
    navbarHasCancelSelectionButton: true

    toolbarHasMainIconsWhenSelecting: false

    inSelectionMode: true
    state: "shown"
    visible: true
    
    hasSelectionOperationsButton: true
    onSelectionOperationsButtonPressed: cyclePopup(selectionMenu);
    
    onSelectionDoneButtonPressed: {
      if (album)
        album.addSelectedMediaSources(mediaCheckerboard.model);
      else
        album = mediaCheckerboard.model.createAlbumFromSelected();

      mediaCheckerboard.unselectAll();
      navStack.goBack()
    }

    onCancelSelectionButtonPressed: {
      mediaCheckerboard.unselectAll();
      navStack.goBack()
    }
    
    SelectionMenu {
      id: selectionMenu
      
      checkerboard: mediaCheckerboard
    }
  }
}

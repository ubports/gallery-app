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

PopupMenu {
  id: selectionMenu
  
  property variant checkerboard
  
  // these are hardcoded to match the location of the Selection Button in the
  // chrome
  popupOriginX: gu(3.5)
  popupOriginY: -gu(6)
  
  visible: false
  state: "hidden"
  
  onPopupInteractionCompleted: {
    hideAllPopups();
  }
  
  onActionInvoked: {
    // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you edit
    // a switch statement in QML.  The short version is: use braces always.
    switch (name) {
      case "SelectAll": {
        if (checkerboard)
          checkerboard.selectAll();
        break;
      }
      
      case "SelectNone": {
        if (checkerboard)
          checkerboard.unselectAll();
        break;
      }
      
      default: {
        console.log("Unknown action", name);
        break;
      }
    }
  }
  
  model: ListModel {
    ListElement {
      title: "Select All"
      action: "SelectAll"
      hasBottomBorder: true
    }
    
    ListElement {
      title: "Select None"
      action: "SelectNone"
    }
  }
}

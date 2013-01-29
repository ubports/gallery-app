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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import "../../Capetown/Widgets"

/*!
*/
PopupMenu {
  id: albumEditMenu
  
  visible: false
  
  model: albumEditList
  
  ListModel {
    id: albumEditList
    
    ListElement {
      title: "Edit album"
      action: "onEdit"
      hasBottomBorder: true
    }
    
    ListElement {
      title: "Export to Device"
      action: "onExport"
      hasBottomBorder: true
    }
    
    ListElement {
      title: "Print album"
      action: "onPrint"
      hasBottomBorder: true
    }
    
    ListElement {
      isSeparator: true
    }
    
    ListElement {
      title: "Share"
      action: "onShare"
      hasBottomBorder: true
      iconFilename: "../../img/icon-share-active.png"
    }
    
    ListElement {
      title: "Delete"
      action: "onDelete"
      iconFilename: "../../img/icon-trash-active.png"
    }
  }
}

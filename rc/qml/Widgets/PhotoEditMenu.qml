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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import "../../Capetown/Widgets"

PopupMenu {
  model: ListModel {
    ListElement {
      title: "Rotate"
      action: "onRotate"
      iconFilename: "../../img/icon-rotate.png"
    }
    ListElement {
      title: "Crop"
      action: "onCrop"
      iconFilename: "../../img/icon-crop.png"
    }
    ListElement {
      title: "Auto enhance"
      action: "onAutoEnhance"
      iconFilename: "../../img/icon-enhance.png"
      hasBottomBorder: true
    }
    ListElement {
      title: "Undo"
      action: "onUndo"
      iconFilename: "../../img/icon-undo.png"
    }
    ListElement {
      title: "Redo"
      action: "onRedo"
      iconFilename: "../../img/icon-redo.png"
      hasBottomBorder: true
    }
    ListElement {
      title: "Revert to original"
      action: "onRevert"
      iconFilename: "../../img/icon-revert.png"
    }
  }
}

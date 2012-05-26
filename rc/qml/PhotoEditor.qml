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
import Gallery 1.0

// A top-level component for editing Photos.
Item {
  id: photoEditor

  // When the user clicks the back button.
  signal closeRequested()

  property alias photo: photoComponent.mediaSource

  // internal
  function leavePhotoEditor() {
    photo.discardSavePoints();
    closeRequested();
  }

  PhotoComponent {
    id: photoComponent

    anchors.top: topToolbar.bottom
    anchors.bottom: bottomToolbar.top
    anchors.left: parent.left
    anchors.right: parent.right

    color: "black"

    ownerName: "PhotoEditor"
  }

  // TODO: use real components for the toolbars/menus instead of this hacky
  // reinvention of the wheel.

  Toolbar {
    id: topToolbar

    anchors.top: parent.top

    ToolbarTextButton {
      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      anchors.leftMargin: gu(2)

      text: "Cancel"

      onPressed: {
        photo.revertToLastSavePoint();
        photoEditor.leavePhotoEditor();
      }
    }

    ToolbarTextButton {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      anchors.rightMargin: gu(2)

      text: "Done"

      onPressed: photoEditor.leavePhotoEditor()
    }
  }

  Toolbar {
    id: bottomToolbar

    anchors.bottom: parent.bottom

    ToolbarIconButton {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      anchors.rightMargin: gu(2)

      selectedIconFilename: "../img/icon-rotate.png"
      deselectedIconFilename: selectedIconFilename

      onPressed: photo.rotateRight()
    }
  }
}

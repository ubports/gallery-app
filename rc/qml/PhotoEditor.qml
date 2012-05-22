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
      anchors.right: parent.right
      anchors.rightMargin: gu(2)

      text: "Done"

      onPressed: photoEditor.closeRequested()
    }
  }

  Toolbar {
    id: bottomToolbar

    anchors.bottom: parent.bottom

    MoreOperationsToolbarButton {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      anchors.rightMargin: gu(2)

      onPressed: editMenu.visible = true
    }
  }

  MouseArea {
    id: cancelArea

    anchors.fill: parent

    visible: editMenu.visible

    onClicked: editMenu.visible = false
  }

  PopupMenu {
    id: editMenu

    popupOriginX: -gu(1.5)
    popupOriginY: -gu(6)

    visible: false

    model: ListModel {
      ListElement {
        title: "Rotate Left"
        action: "onRotateLeft"
      }
      ListElement {
        title: "Rotate Right"
        action: "onRotateRight"
      }
    }

    onActionInvoked: {
      // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you
      // edit a switch statement in QML.  The short version is: use braces
      // always.
      switch (name) {
        case "onRotateLeft": {
          photo.rotateLeft();
          break;
        }

        case "onRotateRight": {
          photo.rotateRight();
          break;
        }
      }
    }

    onPopupInteractionCompleted: visible = false
  }
}

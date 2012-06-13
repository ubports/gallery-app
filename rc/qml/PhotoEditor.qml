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

    Text {
      anchors.centerIn: parent
      color: "#747273"
      text: "Edit"
    }

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
      anchors.left: parent.left
      anchors.leftMargin: gu(2)

      selectedIconFilename: "../img/icon-undo-redo.png"
      deselectedIconFilename: selectedIconFilename

      onPressed: undoMenu.flipVisibility()
    }

    Row {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      anchors.rightMargin: gu(2)

      ToolbarIconButton {
        selectedIconFilename: "../img/icon-rotate.png"
        deselectedIconFilename: selectedIconFilename

        onPressed: photo.rotateRight()
      }

      ToolbarIconButton {
        selectedIconFilename: "../img/icon-crop.png"
        deselectedIconFilename: selectedIconFilename

        onPressed: cropper.show(photo)
      }
    }
  }

  MouseArea {
    id: menuCancelArea

    anchors.fill: parent
    visible: (undoMenu.state == "shown")
    onPressed: undoMenu.state = "hidden"
  }

  PopupMenu {
    id: undoMenu

    popupOriginX: gu(3.5)
    popupOriginY: -gu(6)

    visible: false
    state: "hidden"

    onActionInvoked: {
      // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you edit
      // a switch statement in QML.  The short version is: use braces always.
      switch (name) {
        case "Revert": {
          photoEditor.photo.revertToOriginal();
          state = "hidden";
          break;
        }
      }
    }

    model: ListModel {
      ListElement {
        title: "Revert to original"
        action: "Revert"
      }
    }
  }

  PhotoCropper {
    id: cropper

    function show(photo) {
      enterCropper(photo);
      state = "shown";
    }

    state: "hidden"
    states: [
      State { name: "shown";
        PropertyChanges { target: cropper; visible: true; }
      },
      State { name: "hidden";
        PropertyChanges { target: cropper; visible: false; }
      }
    ]

    anchors.fill: parent

    onCanceled: state = "hidden"

    onCropped: {
      photo.crop(rect);
      state = "hidden";
    }
  }
}

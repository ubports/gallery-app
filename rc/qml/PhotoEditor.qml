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
import "../Capetown"

// A top-level component for editing Photos.
Item {
  id: photoEditor

  // When the user clicks the back button.
  signal closeRequested()

  property alias photo: photoComponent.mediaSource

  function enterEditor(photo) {
    photo.saveState();
    photoEditor.photo = photo;
  }

  Rectangle {
    anchors.fill: parent
    color: "black"

    GalleryPhotoComponent {
      id: photoComponent

      anchors.fill: parent
      anchors.topMargin: topToolbar.height + gu(3)
      anchors.bottomMargin: bottomToolbar.height + gu(3)
      anchors.leftMargin: gu(3)
      anchors.rightMargin: gu(3)

      color: "black"

      ownerName: "PhotoEditor"
    }
  }

  // TODO: use real components for the toolbars/menus instead of this hacky
  // reinvention of the wheel.

  SlidingPane {
    id: topSlidingPane

    x: 0
    y: -height
    width: parent.width
    height: childrenRect.height

    inY: 0

    state: "in"

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
          photo.revertToSavedState();
          photoEditor.closeRequested();
        }
      }

      ToolbarTextButton {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: gu(2)

        text: "Done"

        onPressed: photoEditor.closeRequested()
      }
    }
  }

  SlidingPane {
    id: bottomSlidingPane

    x: 0
    y: parent.height
    width: parent.width
    height: childrenRect.height

    inY: parent.height - height

    state: "in"

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

        ToolbarIconButton {
          selectedIconFilename: "../img/icon-enhance.png"
          deselectedIconFilename: selectedIconFilename

          onPressed: photo.autoEnhance()
        }
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
        case "onUndo": {
          photoEditor.photo.undo();
          state = "hidden";
          break;
        }
        case "onRedo": {
          photoEditor.photo.redo();
          state = "hidden";
          break;
        }
        case "onRevert": {
          photoEditor.photo.revertToOriginal();
          state = "hidden";
          break;
        }
      }
    }

    model: ListModel {
      ListElement {
        title: "Undo"
        action: "onUndo"
        hasBottomBorder: true
      }
      ListElement {
        title: "Redo"
        action: "onRedo"
        hasBottomBorder: true
      }
      ListElement {
        title: "Revert to original"
        action: "onRevert"
      }
    }
  }

  PhotoCropper {
    id: cropper

    function show(photo) {
      topSlidingPane.slideOut();
      bottomSlidingPane.slideOut();

      var ratio_crop_rect = photo.prepareForCropping();
      enterCropper(photo, ratio_crop_rect);
      state = "shown";
    }

    function hide() {
      topSlidingPane.slideIn();
      bottomSlidingPane.slideIn();

      state = "hidden";
    }

    state: "hidden"
    states: [
      State { name: "shown";
        PropertyChanges { target: cropper; opacity: 1; }
      },
      State { name: "hidden";
        PropertyChanges { target: cropper; opacity: 0; }
      }
    ]

    Behavior on opacity {
      animation: NumberAnimation {
        easing.type: Easing.OutQuad
        duration: topSlidingPane.duration
      }
    }

    anchors.fill: parent

    onCanceled: {
      photo.cancelCropping();
      hide();
    }

    onCropped: {
      photo.crop(rect);
      hide();
    }
  }
}

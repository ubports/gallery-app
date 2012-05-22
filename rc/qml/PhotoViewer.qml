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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import uTouch 1.0

Rectangle {
  id: photoViewer
  objectName: "photoViewer"

  // When the user clicks the back button.
  signal closeRequested()
  signal editRequested(variant photo) // The user wants to edit this photo.

  property variant model: null

  // NOTE: These properties should be treated as read-only, as setting them
  // individually can lead to bogus results.  Use setCurrentPhoto() or
  // setCurrentIndex() to initialize the view.
  property variant photo: null
  property alias index: imagePager.currentIndex

  function setCurrentPhoto(photo) {
    imagePager.pageTo(model.indexOf(photo));
  }

  function setCurrentIndex(index) {
    imagePager.pageTo(index);
  }

  color: "black"

  Pager {
    id: imagePager

    property int currentIndexForHighlight: -1

    onMovingChanged: {
      // TODO: if you scroll through a number of pages without stopping, this
      // never gets updated, so the highlighting stops working.
      if (moving)
        currentIndexForHighlight = currentIndex;
    }

    model: parent.model

    delegate: PhotoComponent {
      width: imagePager.width
      height: imagePager.height

      color: "black"
      
      opacity: {
        if (!imagePager.moving || imagePager.contentX < 0
          || index != imagePager.currentIndexForHighlight)
          return 1.0;
        
        return 1.0 - Math.abs((imagePager.contentX - x) / width);
      }
      
      isZoomable: true;

      mediaSource: model.mediaSource
      ownerName: "PhotoViewer"
    }

    // Don't allow flicking while the chrome is actively displaying a popup
    // menu or the image is zoomed. When images are zoomed, mouse drags should
    // pan, not flick.
    interactive: !chrome.popupActive && (currentItem != null) &&
      (currentItem.state == "unzoomed")

    onCurrentIndexChanged: {
      if (model)
        photo = model.getAt(currentIndex);
    }

    UTouchPinchArea {
      anchors.fill: parent;

      onGestureStart: {
        if (chrome.active)
          return;

        chrome.resetVisibility(false);
        imagePager.currentItem.beginPinchZoom();
      }

      onGestureEnd: {
        chrome.resetVisibility(true);
        imagePager.currentItem.endPinchZoom();
      }

      onGestureUpdate: {
        imagePager.currentItem.updatePinchZoom(radius.current / radius.initial);
      }
    }

    MouseArea {
      anchors.fill: parent

      property bool isDragInProgress: false;
      property int dragStartX: -1;
      property int dragStartY: -1;

      Timer {
        id: chromeFadeWaitClock

        interval: 150
        running: false

        onTriggered: chrome.flipVisibility()
      }

      onClicked: {
        var deltaX = mouse.x - dragStartX;
        var deltaY = mouse.y - dragStartY;
        var distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);

        // Trigger chrome if we aren't zoomed or we are but they didn't drag.
        if (parent.currentItem.state == "unzoomed" || distance < 20)
          chromeFadeWaitClock.restart();
      }

      onPressed: {
        if (parent.currentItem.state == "unzoomed")
          return;

        isDragInProgress = true;

        dragStartX = mouse.x;
        dragStartY = mouse.y;

        parent.currentItem.setZoomFocus(
              parent.currentItem.getImageTranslation());
      }

      onPositionChanged: {
        if (isDragInProgress) {
          var deltaX = mouse.x - dragStartX;
          var deltaY = mouse.y - dragStartY;

          parent.currentItem.pan(parent.currentItem.zoomFocusX + deltaX,
            parent.currentItem.zoomFocusY + deltaY);
        }
      }

      onReleased: {
        isDragInProgress = false;
      }

      onDoubleClicked: {
        chromeFadeWaitClock.stop();

        chrome.state = "hidden";

        imagePager.currentItem.zoom(mouse.x, mouse.y);
      }
    }
  }

  ViewerChrome {
    id: chrome

    z: 10
    anchors.fill: parent

    toolbarsAreTranslucent: true

    hasLeftNavigationButton: !imagePager.atXBeginning
    hasRightNavigationButton: !imagePager.atXEnd

    onLeftNavigationButtonPressed: imagePager.pageBack()
    onRightNavigationButtonPressed: imagePager.pageForward()

    popups: [ photoViewerShareMenu, photoViewerOptionsMenu,
      trashOperationDialog, popupAlbumPicker ]

    PhotoViewerShareMenu {
      id: photoViewerShareMenu

      popupOriginX: -gu(8.5)
      popupOriginY: -gu(6)

      onPopupInteractionCompleted: {
        chrome.hideAllPopups();
      }

      visible: false
    }

    PhotoViewerOptionsMenu {
      id: photoViewerOptionsMenu

      popupOriginX: -gu(0.5)
      popupOriginY: -gu(6)

      onPopupInteractionCompleted: {
        chrome.hideAllPopups();
      }

      visible: false

      onActionInvoked: {
        // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you
        // edit a switch statement in QML.  The short version is: use braces
        // always.
        switch (name) {
          case "onEdit": {
            photoViewer.editRequested(photo);
            break;
          }
        }
      }
    }

    DeleteDialog {
      id: trashOperationDialog

      popupOriginX: -gu(24.5)
      popupOriginY: -gu(6)

      onPopupInteractionCompleted: {
        chrome.hideAllPopups();
      }

      visible: false

      onDeleteRequested: {
        model.destroyMedia(photo);

        if (model.count == 0)
          photoViewer.closeRequested();
      }
    }

    PopupAlbumPicker {
      id: popupAlbumPicker

      popupOriginX: -gu(17.5)
      popupOriginY: -gu(6)

      onPopupInteractionCompleted: {
        chrome.hideAllPopups();
      }

      onAlbumPicked: album.addMediaSource(photo)

      visible: false
    }

    onReturnButtonPressed: {
      resetVisibility(false);
      imagePager.currentItem.state = "unzoomed";
      closeRequested();
    }

    onShareOperationsButtonPressed: {
      cyclePopup(photoViewerShareMenu);
    }

    onMoreOperationsButtonPressed: {
      cyclePopup(photoViewerOptionsMenu);
    }

    onAlbumOperationsButtonPressed: {
      cyclePopup(popupAlbumPicker);
    }

    onTrashOperationButtonPressed: {
      cyclePopup(trashOperationDialog);
    }
  }
}

/*
 * Copyright (C) 2011-2012 Canonical Ltd
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

import QtQuick 2.0
import Gallery 1.0
import "../Capetown"
import "../Capetown/Viewer"
import "Components"
import "Widgets"

Rectangle {
  id: viewerWrapper

  property alias photo: galleryPhotoViewer.photo
  property alias model: galleryPhotoViewer.model
  property alias index: galleryPhotoViewer.index
  property alias currentIndexForHighlight:
      galleryPhotoViewer.currentIndexForHighlight
  
  // Set this when entering from an album.
  property variant album
  
  // Read-only
  // Set to true when an image is loaded and displayed.
  property bool isReady: galleryPhotoViewer.currentItem ? 
    galleryPhotoViewer.currentItem.isLoaded : false

  signal closeRequested()
  signal editRequested(variant photo)

  function setCurrentIndex(index) {
    galleryPhotoViewer.setCurrentIndex(index);
    chrome.resetVisibility(true);
  }

  function setCurrentPhoto(photo) {
    galleryPhotoViewer.setCurrentPhoto(photo);
    chrome.resetVisibility(true);
  }

  function goBack() {
    galleryPhotoViewer.goBack();
  }

  function goForward() {
    galleryPhotoViewer.goForward();
  }
  
  anchors.fill: parent;

  color: "black";

  PhotoViewer {
    id: galleryPhotoViewer

    // When the user clicks the back button.
    signal closeRequested()
    signal editRequested(variant photo) // The user wants to edit this photo.

    // NOTE: These properties should be treated as read-only, as setting them
    // individually can lead to bogus results.  Use setCurrentPhoto() or
    // setCurrentIndex() to initialize the view.
    property variant photo: null

    function setCurrentPhoto(photo) {
      setCurrentIndex(model.indexOf(photo));
    }

    function goBack() {
      galleryPhotoViewer.currentItem.state = "unzoomed";
      pageBack();
    }

    function goForward() {
      galleryPhotoViewer.currentItem.state = "unzoomed";
      pageForward();
    }

    anchors.fill: parent

    Connections {
      target: photo || null
      ignoreUnknownSignals: true
      onBusyChanged: galleryPhotoViewer.updateBusy()
    }

    // Internal: use to switch the busy indicator on or off.
    function updateBusy() {
      if (photo.busy) {
        busySpinner.visible = true;
        // HACK: chrome.hide() doesn't work here for some reason.
        chrome.visible = false;
      } else {
        busySpinner.visible = false;
        chrome.visible = true;
      }
    }

    onCurrentIndexChanged: {
      if (model)
        photo = model.getAt(currentIndex);
    }

    delegate: ZoomablePhotoComponent {
      id: galleryPhotoComponent

      width: galleryPhotoViewer.width
      height: galleryPhotoViewer.height

      visible: true
      color: "black"

      opacity: {
        if (!galleryPhotoViewer.moving || galleryPhotoViewer.contentX < 0
          || index != galleryPhotoViewer.currentIndexForHighlight)
          return 1.0;

        return 1.0 - Math.abs((galleryPhotoViewer.contentX - x) / width);
      }

      mediaSource: model.mediaSource
      ownerName: "galleryPhotoViewer"

      onClicked: chromeFadeWaitClock.restart();
      onZoomed: {
        chromeFadeWaitClock.stop();
        chrome.hide(true);
      }
      onUnzoomed: {
        chromeFadeWaitClock.stop();
        chrome.hide(true);
      }
      
      Image {
        id: previewOverlay
        
        anchors.fill: parent
        
        fillMode: Image.PreserveAspectFit
        
        visible: !galleryPhotoComponent.isLoaded
      }
      
      onIsLoadedChanged: {
        if (isLoaded)
          previewOverlay.source = mediaSource.galleryPath;
      }
    }

    // Don't allow flicking while the chrome is actively displaying a popup
    // menu, or the image is zoomed, or we're cropping. When images are zoomed,
    // mouse drags should pan, not flick.
    interactive: !chrome.popupActive && (currentItem != null) &&
                 (currentItem.state == "unzoomed") && cropper.state == "hidden"

    Timer {
      id: chromeFadeWaitClock

      interval: 100
      running: false

      onTriggered: chrome.flipVisibility(true)
    }

    AnimatedImage {
      id: busySpinner

      visible: false
      anchors.centerIn: parent
      source: "../img/spin.mng"
    }

    ViewerChrome {
      id: chrome

      z: 10
      anchors.fill: parent

      autoHideWait: 8000

      toolbarsAreTextured: false
      toolbarsAreTranslucent: true
      toolbarsAreDark: true

      toolbarHasEditOperationsButton: true

      hasLeftNavigationButton: !galleryPhotoViewer.atXBeginning
      hasRightNavigationButton: !galleryPhotoViewer.atXEnd

      onLeftNavigationButtonPressed: galleryPhotoViewer.goBack()
      onRightNavigationButtonPressed: galleryPhotoViewer.goForward()

      popups: [ photoViewerShareMenu, photoViewerOptionsMenu,
        trashOperationDialog, trashOrRemoveOperationDialog, popupAlbumPicker,
        editMenu ]

      GenericShareMenu {
        id: photoViewerShareMenu

        popupOriginX: -gu(8.5)
        popupOriginY: -gu(6)
        
        onActionInvoked: {
          switch (name) {
            case "onQuickShare": {
              shareImage(photo);
            }
          }
        }

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
      
      // Shown when launched from event view.
      DeleteDialog {
        id: trashOperationDialog
        
        actionTitle: "Delete Photo"
        
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
      
      // Shown when launched from album view.
      DeleteRemoveDialog {
        id: trashOrRemoveOperationDialog
        
        // internal
        function finishRemove() {
          if (model.count === 0)
            photoViewer.closeRequested();
        }
        
        action0Title: "Remove from album"
        action1Title: "Delete photo"
        
        popupOriginX: -gu(24.5)
        popupOriginY: -gu(6)
        
        visible: false
        
        onRemoveRequested: {
          viewerWrapper.album.removeMediaSource(photo);
          finishRemove();
        }
        
        onDeleteRequested: {
          model.destroyMedia(photo);
          finishRemove();
        }
        
        onPopupInteractionCompleted: chrome.hideAllPopups()
      }

      PhotoEditMenu {
        id: editMenu

        popupOriginX: gu(3.5)
        popupOriginY: -gu(6)

        onPopupInteractionCompleted: chrome.hideAllPopups()

        visible: false

        onActionInvoked: {
          // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you edit
          // a switch statement in QML.  The short version is: use braces always.
          switch (name) {
            case "onRotate": {
              state = "hidden";
              photo.rotateRight();
              break;
            }
            case "onCrop": {
              state = "hidden";
              galleryPhotoViewer.visible = false
              cropper.show(photo);
              break;
            }
            case "onAutoEnhance": {
              state = "hidden";
              photo.autoEnhance();
              break;
            }
            case "onUndo": {
              state = "hidden";
              photo.undo();
              break;
            }
            case "onRedo": {
              state = "hidden";
              photo.redo();
              break;
            }
            case "onRevert": {
              state = "hidden";
              photo.revertToOriginal();
              break;
            }
          }
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
        galleryPhotoViewer.currentItem.state = "unzoomed";
        closeRequested();
      }

      onShareOperationsButtonPressed: cyclePopup(photoViewerShareMenu)
      onMoreOperationsButtonPressed: cyclePopup(photoViewerOptionsMenu)
      onAlbumOperationsButtonPressed: cyclePopup(popupAlbumPicker)
      onTrashOperationButtonPressed: cyclePopup(album ? trashOrRemoveOperationDialog : trashOperationDialog)
      onEditOperationsButtonPressed: cyclePopup(editMenu)
    }

    onCloseRequested: viewerWrapper.closeRequested()
    onEditRequested: viewerWrapper.editRequested(photo)
  }

  CropInteractor {
    id: cropper

    function show(photo) {
      chrome.hide(true);

      var ratio_crop_rect = photo.prepareForCropping();
      enterCropper(photo, ratio_crop_rect);
      state = "shown";
    }

    function hide() {
      state = "hidden";
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

    MouseArea {
      id: blocker
      
      visible: cropper.state == "shown"
      anchors.fill: parent
      
      onClicked: { }
    }

    onCanceled: {
      photo.cancelCropping();
      hide();
      galleryPhotoViewer.visible = true;
    }

    onCropped: {
      var qtRect = Qt.rect(rect.x, rect.y, rect.width, rect.height);
      photo.crop(qtRect);
      hide();
      galleryPhotoViewer.visible = true;
    }
  }
}

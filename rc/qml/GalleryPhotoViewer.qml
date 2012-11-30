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
import Ubuntu.Components 0.1
import "../Capetown"
import "../Capetown/Viewer"
import "Components"
import "Widgets"
import "../js/Gallery.js" as Gallery

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
  //
  // NOTE: The empty-model check does perform a useful function here and should NOT be
  // removed; for whatever reason, it's possible to get here and have what would have
  // been the current item be deleted, but not be null, and what it actually points to
  // is no longer valid and will result in an immediate segfault if dereferenced.
  //
  // Since there is no current item if there are no more photo objects left in the model,
  // the check catches this before we can inadvertently follow a stale pointer.
  property bool isReady: model != null && model.count > 0 &&
    (galleryPhotoViewer.currentItem ? galleryPhotoViewer.currentItem.isLoaded : false)

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

    // Used for supporting swiping from the bottom of the display upward;
    // prevent the app from interpreting it as prev/next photo and force
    // the toolbar to show.
    MouseArea {
      enabled: !(chrome.visible)
      preventStealing: true

      anchors.bottom: parent.bottom
      width: parent.width
      height: units.gu(1)

      onReleased: chrome.show(true)
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

      // TODO: re-enable navigation buttons; we've removed them here because
      //       they're not desired for the CES phone demo
      hasLeftNavigationButton: false
      hasRightNavigationButton: false

      onLeftNavigationButtonPressed: galleryPhotoViewer.goBack()
      onRightNavigationButtonPressed: galleryPhotoViewer.goForward()

      popups: [ photoViewerShareMenu, photoViewerOptionsMenu,
        trashOperationDialog, trashOrRemoveOperationDialog, popupAlbumPicker,
        editMenu ]

      GenericShareMenu {
        id: photoViewerShareMenu

        popupOriginX: -units.gu(8.5)
        popupOriginY: -units.gu(6)

        onActionInvoked: {
          switch (name) {
            case "onQuickShare": {
              shareImage(photo);
              break;
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

        popupOriginX: -units.gu(0.5)
        popupOriginY: -units.gu(6)

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
        
        popupOriginX: -units.gu(24.5)
        popupOriginY: -units.gu(6)

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
        
        popupOriginX: -units.gu(24.5)
        popupOriginY: -units.gu(6)
        
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

        popupOriginX: units.gu(3.5)
        popupOriginY: -units.gu(6)

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

        popupOriginX: -units.gu(17.5)
        popupOriginY: -units.gu(6)

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
    
    property var targetPhoto

    function show(photo) {
      chrome.hide(true);
      
      targetPhoto = photo;
      
      fadeOutPhotoAnimation.running = true;
    }

    function hide() {
      state = "hidden";
      galleryPhotoViewer.opacity = 0.0;
      galleryPhotoViewer.visible = true;
      fadeInPhotoAnimation.running = true;      
    }

    state: "hidden"
    states: [
      State { name: "shown";
        PropertyChanges { target: cropper; visible: true; }
        PropertyChanges { target: cropper; opacity: 1.0; }
      },
      State { name: "hidden";
        PropertyChanges { target: cropper; visible: false; }
        PropertyChanges { target: cropper; opacity: 0.0; }
      }
    ]
    
    Behavior on opacity {
      NumberAnimation { duration: Gallery.FAST_DURATION }
    }

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

      targetPhoto = null;
    }

    onCropped: {
      var qtRect = Qt.rect(rect.x, rect.y, rect.width, rect.height);
      photo.crop(qtRect);

      hide();

      targetPhoto = null;
    }
    
    onOpacityChanged: {
      if (opacity == 1.0)
        galleryPhotoViewer.visible = false
    }
    
    NumberAnimation {
      id: fadeOutPhotoAnimation
      
      from: 1.0
      to: 0.0
      target: galleryPhotoViewer
      property: "opacity"
      duration: Gallery.FAST_DURATION
      easing.type: Easing.InOutQuad
      
      onRunningChanged: {
        if (running == false) {
          var ratio_crop_rect = cropper.targetPhoto.prepareForCropping();
          cropper.enter(cropper.targetPhoto, ratio_crop_rect);
          cropper.state = "shown";
        }
      }
    }

    NumberAnimation {
      id: fadeInPhotoAnimation
      
      from: 0.0
      to: 1.0
      target: galleryPhotoViewer
      property: "opacity"
      duration: Gallery.FAST_DURATION
      easing.type: Easing.InOutQuad    
    }
  }
}

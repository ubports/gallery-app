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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import "../Capetown"
import "../Capetown/Viewer"
import "Components"
import "Utility"
import "Widgets"
import "../js/Gallery.js" as Gallery

/*!
*/
Item {
  id: viewerWrapper

  /*!
  */
  property alias photo: galleryPhotoViewer.photo
  /*!
  */
  property alias model: galleryPhotoViewer.model
  /*!
  */
  property alias index: galleryPhotoViewer.index
  /*!
  */
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

  /*!
  */
  signal closeRequested()
  /*!
  */
  signal editRequested(variant photo)

  /*!
  */
  function setCurrentIndex(index) {
    galleryPhotoViewer.setCurrentIndex(index);
  }

  /*!
  */
  function setCurrentPhoto(photo) {
    galleryPhotoViewer.setCurrentPhoto(photo);
  }

  /*!
  */
  function goBack() {
    galleryPhotoViewer.goBack();
  }

  /*!
  */
  function goForward() {
    galleryPhotoViewer.goForward();
  }

  Rectangle{
    color: "black"
    anchors.fill: parent
  }

  property alias tools: galleryPhotoViewer.tools
  PhotoViewer {
    id: galleryPhotoViewer
    objectName: "photoViewer"

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
      galleryPhotoViewer.currentItem.unzoom();
      pageBack();
    }

    function goForward() {
      galleryPhotoViewer.currentItem.unzoom();
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
      } else {
        busySpinner.visible = false;
      }
    }

    onCurrentIndexChanged: {
      if (model)
        photo = model.getAt(currentIndex);
      //chromeBar.setBarShown(false); // TODO: hide toolbar
    }

    delegate: PhotoViewerDelegate {
      id: viewerDelegate

      width: galleryPhotoViewer.width
      height: galleryPhotoViewer.height
      useInteractivePreview: galleryPhotoViewer.moving

      visible: true

      opacity: {
        if (!galleryPhotoViewer.moving || galleryPhotoViewer.contentX < 0
          || index != galleryPhotoViewer.currentIndexForHighlight)
          return 1.0;

        return 1.0 - Math.abs((galleryPhotoViewer.contentX - x) / width);
      }
      
      mediaSource: model.mediaSource

      onClicked: chromeFadeWaitClock.restart()
      onZoomed: {
          chromeFadeWaitClock.stop();
          chromeBar.setBarShown(false);
      }
      onUnzoomed: {
          chromeFadeWaitClock.stop();
          //chromeBar.setBarShown(false);
      }
    }

    // Don't allow flicking while the chrome is actively displaying a popup
    // menu, or the image is zoomed, or we're cropping. When images are zoomed,
    // mouse drags should pan, not flick.
    interactive: (currentItem != null) &&
                 (currentItem.state == "unzoomed") && cropper.state == "hidden"
                 && !editHUD.actionActive  // this is a hack for preventing zoom from happening when tapping on HUD
                                           // as currently the events get through to gallery also


    Timer {
      id: chromeFadeWaitClock

      interval: 250
      running: false

      //onTriggered: chromeBar.setBarShown(!chromeBar.showChromeBar)
    }

    ActivityIndicator {
        id: busySpinner
        anchors.centerIn: parent
        visible: false
        running: visible
    }

    property ToolbarActions tools: ToolbarActions {
       Action {
            text: "Edit"
            iconSource: "../img/edit.png"
            onTriggered: {
                editPopover.caller = caller;
                editPopover.show();
            }
        }
        Action {
            text: "Add"
            enabled: false
            iconSource: "../img/add.png"
            //chromeBar.setBarShown(false);
            //popupAlbumPicker.visible = true;
        }
        Action {
            text: "Delete"
            iconSource: "../img/delete.png"
            onTriggered: {
                deletePopover.caller = caller;
                deletePopover.show();
            }
        }
        Action {
            text: "Share"
            iconSource: "../img/share.png"
            onTriggered: {
                sharePopover.picturePath = viewerWrapper.photo.path;
                sharePopover.caller = caller;
                sharePopover.show();
            }
        }

        back: Action {
            text: "Back"
            iconSource: "../img/back.png"
            onTriggered: {
                galleryPhotoViewer.currentItem.unzoom();
                closeRequested();
            }
        }
    }

        SharePopover {
            id: sharePopover
            objectName: "sharePopover"
            visible: false
        }

        DeletePopover {
            function finishRemove() {
                if (!album === undefined) return;
                if (model.count === 0) photoViewer.closeRequested();
            }

            visible: false
            id: deletePopover
            objectName: "deletePopover"

            onDeleteClicked: {
                viewerWrapper.model.destroyMedia(photo);
                galleryPhotoViewer.currentIndexChanged();
                deletePopover.finishRemove();
                deletePopover.hide();
            }
        }

        EditPopover {
            id: editPopover
            objectName: "editPopover"
            visible: false
            photo: galleryPhotoViewer.photo
            cropper: viewerWrapper.cropper
        }

    PopupAlbumPicker {
        id: popupAlbumPicker
        objectName: "popupAlbumPicker"

        popupOriginX: -units.gu(17.5)
        popupOriginY: -units.gu(6)

        onPopupInteractionCompleted: {
            visible = false;
        }

        onAlbumPicked: album.addMediaSource(photo)

        visible: false
    }

    onCloseRequested: viewerWrapper.closeRequested()
    onEditRequested: viewerWrapper.editRequested(photo)
  }

  property alias cropper: cropper
  CropInteractor {
    id: cropper
    
    property var targetPhoto

    function show(photo) {
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

  EditPreview {
      id: editPreview
      anchors.fill: parent
      source: galleryPhotoViewer.photo.galleryPreviewPath

      visible: editHUD.actionActive

      exposure: editHUD.exposureValue

      brightness: editHUD.brightness
      contrast: editHUD.contrast
      saturation: editHUD.saturation
      hue: editHUD.hue
  }

  EditingHUD {
      id: editHUD
      photo: galleryPhotoViewer.photo
      onExposureActivated: editPreview.useExposure()
      onColorBalanceActivated: editPreview.useColorBalance()
  }
}

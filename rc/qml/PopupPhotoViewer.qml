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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import "../Capetown"

// A PhotoViewer that is opened and closed with the PhotoViewerTransition.
Item {
  id: popupPhotoViewer

  property alias model: viewer.model

  property alias photo: viewer.photo
  property alias index: viewer.index
  
  // Read-only
  property bool animationRunning: transition.animationRunning || 
    fadeIn.running || fadeOut.running
  property alias isPoppedUp: viewer.visible
  
  function setCurrentPhoto(photo) {
    viewer.setCurrentPhoto(photo);
  }

  function setCurrentIndex(index) {
    viewer.setCurrentIndex(index);
  }
  
  signal opening()
  signal opened() // The photo viewer's opening animation is finished.
  signal closeRequested() // The user pressed the back button.  Call animateClosed() or close().
  signal closed() // The photo viewer's closing animation is finished.
  signal editRequested(variant photo) // The user wants to edit this photo.

  function animateOpen(photo, thumbnailRect, adjustForPhotoMat) {
    opening();
    viewer.setCurrentPhoto(photo);
    transition.transitionToPhotoViewer(photo, thumbnailRect, adjustForPhotoMat);
  }

  function animateClosed(thumbnailRect, adjustForPhotoMat) {
    transition.transitionFromPhotoViewer(photo, thumbnailRect, adjustForPhotoMat);
    viewer.visible = false;
  }

  function fadeOpen(photo) {
    opening();
    viewer.setCurrentPhoto(photo);
    fadeIn.restart();
  }

  function fadeClosed() {
    fadeOut.restart();
  }

  function close() {
    viewer.visible = false;
    closed();
  }
  
  GalleryPhotoViewer {
    id: viewer

    anchors.fill: parent
    visible: false

    onCloseRequested: popupPhotoViewer.closeRequested()

    onEditRequested: popupPhotoViewer.editRequested(photo)
  }

  PhotoViewerTransition {
    id: transition

    anchors.fill: parent

    onTransitionToPhotoViewerCompleted: {
      viewer.visible = true;
      opened();
    }

    onTransitionFromPhotoViewerCompleted: {
      closed();
    }
  }

  FadeInAnimation {
    id: fadeIn

    target: viewer
    onRunningChanged: {
      if (!running)
        opened();
    }
  }

  FadeOutAnimation {
    id: fadeOut

    target: viewer
    onRunningChanged: {
      if (!running)
        closed();
    }
  }
}

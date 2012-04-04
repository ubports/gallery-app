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

import QtQuick 1.1
import Gallery 1.0
import "../Capetown"

// Some custom components and animations that we want to invoke whenever we
// bring up the album viewer.
Item {
  id: albumViewerTransition
  
  property Album album
  property Rectangle backgroundGlass
  property int durationMsec: 500
  
  signal transitionToAlbumViewerCompleted()
  signal transitionFromAlbumViewerCompleted()
  signal dissolveCompleted(variant fadeOutTarget, variant fadeInTarget)

  function transitionToAlbumViewer(album, thumbnailRect) {
    albumViewerTransition.album = album;

    expandAlbum.x = thumbnailRect.x;
    expandAlbum.y = thumbnailRect.y;
    expandAlbum.width = thumbnailRect.width;
    expandAlbum.height = thumbnailRect.height;

    showAlbumViewerAnimation.screenRect = getFullscreenRect();
    showAlbumViewerAnimation.start();
  }

  function transitionFromAlbumViewer(album, thumbnailRect) {
    albumViewerTransition.album = album;

    var rect = getFullscreenRect();
    expandAlbum.x = rect.x;
    expandAlbum.y = rect.y;
    expandAlbum.width = rect.width;
    expandAlbum.height = rect.height;

    hideAlbumViewerAnimation.thumbnailRect = thumbnailRect;
    hideAlbumViewerAnimation.start();
  }

  function dissolve(fadeOutTarget, fadeInTarget) {
    dissolveAlbumViewerTransition.fadeOutTarget = fadeOutTarget;
    dissolveAlbumViewerTransition.fadeInTarget = fadeInTarget;
    dissolveAlbumViewerTransition.start();
  }

  // internal
  function getFullscreenRect() {
    var rect = {"x": 0, "y": 0, "width": 0, "height": 0};

    // We have to compensate for the frame, present in the animation but not in
    // the album viewer.
    var frameWidth = width / 2 * expandAlbum.frameToContentWidth;
    var frameHeight = height * expandAlbum.frameToContentHeight;

    rect.x = (width - frameWidth) / 2; // Centered.
    rect.y = (height - frameHeight) / 2;
    rect.width = frameWidth;
    rect.height = frameHeight;

    return rect;
  }

  AlbumOpener {
    id: expandAlbum
    
    album: parent.album
    isPreview: true
    contentHasPreviewFrame: true
    
    visible: false
  }

  SequentialAnimation {
    id: showAlbumViewerAnimation

    property variant screenRect: {"x": 0, "y": 0, "width": 0, "height": 0}

    PropertyAction { target: expandAlbum; property: "visible"; value: true; }

    ParallelAnimation {
      ExpandAnimation {
        target: expandAlbum
        endX: showAlbumViewerAnimation.screenRect.x
        endY: showAlbumViewerAnimation.screenRect.y
        endWidth: showAlbumViewerAnimation.screenRect.width
        endHeight: showAlbumViewerAnimation.screenRect.height
        duration: durationMsec
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "openFraction"
        from: (album && album.closed ? 0 : 1)
        to: 0.5
        duration: durationMsec
        easing.type: Easing.OutQuad
      }

      FadeInAnimation {
        target: backgroundGlass
        duration: durationMsec
      }
    }

    PropertyAction { target: expandAlbum; property: "visible"; value: false; }
    PropertyAction { target: backgroundGlass; property: "visible"; value: false; }

    ScriptAction {
      script: {
        album.closed = false;
        if (album.currentPage == album.firstValidCurrentPage)
          album.currentPage = album.firstContentPage;
      }
    }

    onCompleted: {
      transitionToAlbumViewerCompleted();
    }
  }

  SequentialAnimation {
    id: hideAlbumViewerAnimation

    property variant thumbnailRect: {"x": 0, "y": 0, "width": 0, "height": 0}

    PropertyAction { target: expandAlbum; property: "visible"; value: true; }
    
    ParallelAnimation {
      ExpandAnimation {
        target: expandAlbum
        endX: hideAlbumViewerAnimation.thumbnailRect.x
        endY: hideAlbumViewerAnimation.thumbnailRect.y
        endWidth: hideAlbumViewerAnimation.thumbnailRect.width
        endHeight: hideAlbumViewerAnimation.thumbnailRect.height
        duration: durationMsec
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "openFraction"
        from: 0.5
        to: 1
        duration: durationMsec
        easing.type: Easing.InQuad
      }

      FadeOutAnimation {
        target: backgroundGlass
        duration: durationMsec
      }
    }

    PropertyAction { target: expandAlbum; property: "visible"; value: false; }

    onCompleted: {
      transitionFromAlbumViewerCompleted();
    }
  }

  DissolveAnimation {
    id: dissolveAlbumViewerTransition

    fadeOutTarget: Rectangle { } // Dummy Rectangle to avoid compilation errors.
    fadeInTarget: Rectangle { } // Dummy Rectangle to avoid compilation errors.

    onCompleted: {
      dissolveCompleted(fadeOutTarget, fadeInTarget);
    }
  }
}

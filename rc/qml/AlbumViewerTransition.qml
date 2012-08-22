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
  
  signal transitionToAlbumViewerCompleted()
  signal transitionFromAlbumViewerCompleted()
  signal dissolveCompleted(variant fadeOutTarget, variant fadeInTarget)

  property Album album
  property Rectangle backgroundGlass
  property int duration: 500
  property int pagesPerSpread

  // internal
  property bool hideStayingOpen
  
  function transitionToAlbumViewer(album, thumbnailRect) {
    albumViewerTransition.album = album;

    expandAlbum.x = thumbnailRect.x;
    expandAlbum.y = thumbnailRect.y;
    expandAlbum.width = thumbnailRect.width;
    expandAlbum.height = thumbnailRect.height;

    showAlbumViewerAnimation.screenRect = getFullscreenRect();
    showAlbumViewerAnimation.start();
  }

  function transitionFromAlbumViewer(album, thumbnailRect, stayOpen) {
    albumViewerTransition.album = album;

    var rect = getFullscreenRect();
    expandAlbum.x = rect.x;
    expandAlbum.y = rect.y;
    expandAlbum.width = rect.width;
    expandAlbum.height = rect.height;

    hideStayingOpen = stayOpen;
    hideAlbumViewerAnimation.thumbnailRect = thumbnailRect;
    hideAlbumViewerAnimation.start();
  }

  function dissolve(fadeOutTarget, fadeInTarget) {
    dissolveAlbumViewerTransition.fadeOutTarget = fadeOutTarget || dissolveDummy;
    dissolveAlbumViewerTransition.fadeInTarget = fadeInTarget || dissolveDummy;
    dissolveAlbumViewerTransition.start();
  }

  // internal
  function getFullscreenRect() {
    var rect = {"x": 0, "y": 0, "width": 0, "height": 0};

    // We have to compensate for the frame, present in the animation but not in
    // the album viewer.
    var frameWidth = width / pagesPerSpread * expandAlbum.frameToContentWidth;
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
    showAsPortrait: application.isPortrait
    
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
        duration: albumViewerTransition.duration
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "openFraction"
        from: isPortrait ? 1 : (album && album.closed ? 0 : 1)
        to: isPortrait ? 1 : 0.5
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "topMargin"
        from: expandAlbum.previewTopMargin
        to: expandAlbum.pageTopMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "bottomMargin"
        from: expandAlbum.previewBottomMargin
        to: expandAlbum.pageBottomMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "gutterMargin"
        from: expandAlbum.previewGutterMargin
        to: expandAlbum.pageGutterMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "outerMargin"
        from: expandAlbum.previewOuterMargin
        to: expandAlbum.pageOuterMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "insideMargin"
        from: expandAlbum.previewInsideMargin
        to: expandAlbum.pageInsideMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }

      FadeInAnimation {
        target: backgroundGlass
        duration: albumViewerTransition.duration
      }
    }

    PropertyAction { target: expandAlbum; property: "visible"; value: false; }
    PropertyAction { target: backgroundGlass; property: "visible"; value: false; }

    onCompleted: {
      album.closed = false;
      if (album.currentPage == album.firstValidCurrentPage)
        album.currentPage = album.firstContentPage;

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
        duration: albumViewerTransition.duration
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "openFraction"
        from: isPortrait ? (hideStayingOpen ? 1 : 0) : 0.5
        to: (hideStayingOpen ? 1 : 0)
        duration: albumViewerTransition.duration
        easing.type: Easing.InQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "topMargin"
        from: expandAlbum.pageTopMargin
        to: expandAlbum.previewTopMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "bottomMargin"
        from: expandAlbum.pageBottomMargin
        to: expandAlbum.previewBottomMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "gutterMargin"
        from: expandAlbum.pageGutterMargin
        to: expandAlbum.previewGutterMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "outerMargin"
        from: expandAlbum.pageOuterMargin
        to: expandAlbum.previewOuterMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }
      NumberAnimation {
        target: expandAlbum
        property: "insideMargin"
        from: expandAlbum.pageInsideMargin
        to: expandAlbum.previewInsideMargin
        duration: albumViewerTransition.duration
        easing.type: Easing.OutQuad
      }

      FadeOutAnimation {
        target: backgroundGlass
        duration: albumViewerTransition.duration
      }
    }

    PropertyAction { target: expandAlbum; property: "visible"; value: false; }

    onCompleted: {
      album.closed = !hideStayingOpen;

      transitionFromAlbumViewerCompleted();
    }
  }

  DissolveAnimation {
    id: dissolveAlbumViewerTransition

    fadeOutTarget: dissolveDummy
    fadeInTarget: dissolveDummy

    onCompleted: {
      dissolveCompleted(fadeOutTarget, fadeInTarget);
    }
  }

  Item {
    id: dissolveDummy
  }
}

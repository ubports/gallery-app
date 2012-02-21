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

// Some custom components and animations that we want to invoke whenever we
// bring up the album viewer.
Item {
  id: albumViewerTransition
  
  property int thumbnailGutter: gu(1)
  property int pageGutter: gu(3)
  
  function transitionToAlbumViewer(album, thumbnailRect) {
    var translatedRect = mapFromItem(application, thumbnailRect.x, thumbnailRect.y);
    translatedRect.width = thumbnailRect.width;
    translatedRect.height = thumbnailRect.height;

    expandAlbum.album = album;
    expandAlbum.x = translatedRect.x;
    expandAlbum.y = translatedRect.y;
    expandAlbum.width = translatedRect.width;
    expandAlbum.height = translatedRect.height;

    showAlbumViewerAnimation.start();
  }

  function transitionFromAlbumViewer(album, thumbnailRect) {
    var translatedRect = mapFromItem(application, thumbnailRect.x, thumbnailRect.y);
    translatedRect.width = thumbnailRect.width;
    translatedRect.height = thumbnailRect.height;

    expandAlbum.album = album;
    hideAlbumViewerAnimation.thumbnailRect = translatedRect;
    hideAlbumViewerAnimation.start();
  }

  function dissolveFromAlbumViewer(fadeOutTarget, fadeInTarget) {
    dissolveAlbumViewerTransition.fadeOutTarget = fadeOutTarget;
    dissolveAlbumViewerTransition.fadeInTarget = fadeInTarget;
    dissolveAlbumViewerTransition.start();
  }

  signal transitionToAlbumViewerCompleted()
  signal transitionFromAlbumViewerCompleted()
  signal dissolveFromAlbumViewerCompleted()

  AlbumPreviewComponent {
    id: expandAlbum
    
    visible: false
  }

  SequentialAnimation {
    id: showAlbumViewerAnimation

    PropertyAction { target: expandAlbum; property: "visible"; value: true; }
    ParallelAnimation {
      ExpandAnimation {
        target: expandAlbum
        endHeight: albumViewerTransition.height + expandAlbum.nameHeight
        duration: 200
      }

      NumberAnimation {
        target: expandAlbum
        property: "pageGutter"
        from: thumbnailGutter
        to: pageGutter
        duration: 200
      }

      NumberAnimation {
        target: expandAlbum
        property: "bookmarkOpacity"
        to: 0
        duration: 200
      }
    }

    PropertyAction { target: expandAlbum; property: "visible"; value: false; }
    PropertyAction { target: expandAlbum; property: "bookmarkOpacity"; value: 1; }

    onCompleted: {
      transitionToAlbumViewerCompleted();
    }
  }

  SequentialAnimation {
    id: hideAlbumViewerAnimation

    property variant thumbnailRect: {"x": 0, "y": 0, "width": 0, "height": 0}

    PropertyAction { target: expandAlbum; property: "bookmarkOpacity"; value: 0; }
    PropertyAction { target: expandAlbum; property: "visible"; value: true; }
    ParallelAnimation {
      ExpandAnimation {
        target: expandAlbum
        endX: hideAlbumViewerAnimation.thumbnailRect.x
        endY: hideAlbumViewerAnimation.thumbnailRect.y
        endWidth: hideAlbumViewerAnimation.thumbnailRect.width
        endHeight: hideAlbumViewerAnimation.thumbnailRect.height
        duration: 200
        easingType: Easing.OutQuad
      }

      NumberAnimation {
        target: expandAlbum
        property: "pageGutter"
        from: pageGutter
        to: thumbnailGutter
        duration: 200
      }

      NumberAnimation {
        target: expandAlbum
        property: "bookmarkOpacity"
        to: 1
        duration: 200
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
      dissolveFromAlbumViewerCompleted();
    }
  }
}

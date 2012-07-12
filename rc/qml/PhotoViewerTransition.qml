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
import "../Capetown"

// Some custom components and animations that we want to invoke whenever we
// bring up the photo viewer.
Item {
  id: photoViewerTransition
  objectName: "photoViewerTransition"

  function transitionToPhotoViewer(photo, thumbnailRect, adjustForPhotoMat) {
    if (adjustForPhotoMat)
      thumbnailRect = adjustRectForPhotoMat(thumbnailRect);

    expandPhoto.setOverThumbnail(photo, thumbnailRect);
    showPhotoViewerAnimation.start();
  }

  function transitionFromPhotoViewer(photo, thumbnailRect, adjustForPhotoMat) {
    if (adjustForPhotoMat)
      thumbnailRect = adjustRectForPhotoMat(thumbnailRect);

    expandPhoto.setOverThumbnail(photo, thumbnailRect);
    hidePhotoViewerAnimation.thumbnailRect =
      Qt.rect(expandPhoto.x, expandPhoto.y, expandPhoto.width, expandPhoto.height);

    // Start part of the way in so the jump to the scaled thumbnail isn't so noticeable.
    expandPhoto.x /= 10;
    expandPhoto.y /= 10;
    expandPhoto.width = parent.width - expandPhoto.x * 2;
    expandPhoto.height = parent.height - expandPhoto.y * 2;
    hidePhotoViewerAnimation.start();
  }

  // internal
  function adjustRectForPhotoMat(rect) {
    // The photo is actually slightly smaller than the delegate rect, due to
    // the photo mat we apply to the previews.
    var thumbnailPhotoWidth = 182; // These dimensions come from the mat image.
    var thumbnailPhotoHeight = 134;

    rect.x += (rect.width - thumbnailPhotoWidth) / 2;
    rect.y += (rect.height - thumbnailPhotoHeight) / 2;
    rect.width = thumbnailPhotoWidth;
    rect.height = thumbnailPhotoHeight;

    return rect;
  }

  signal transitionToPhotoViewerCompleted()
  signal transitionFromPhotoViewerCompleted()

  Rectangle {
    id: fadeRectangle

    visible: false
    color: "#444444"
    anchors.fill: parent
  }

  GalleryPhotoComponent {
    id: expandPhoto

    visible: false
    isCropped: false
    isPreview: true
    isAnimate: true
    ownerName: "PhotoViewerTransition"
    
    color: "transparent"

    function setOverThumbnail(photo, thumbnailRect) {
      mediaSource = photo;

      x = thumbnailRect.x;
      y = thumbnailRect.y;
      width = thumbnailRect.width;
      height = thumbnailRect.height;

      // Expand the photo based on its real aspect ratio.
      if (width == paintedWidth) {
        var oldWidth = width;
        width *= height / paintedHeight;
        x += (oldWidth - width) / 2;
      } else {
        var oldHeight = height;
        height *= width / paintedWidth;
        y += (oldHeight - height) / 2;
      }
    }
  }

  SequentialAnimation {
    id: showPhotoViewerAnimation

    ParallelAnimation {
      SequentialAnimation {
        FadeInAnimation { target: expandPhoto; duration: 40; }
        ExpandAnimation { target: expandPhoto; duration: 160; }
      }
      FadeInAnimation { target: fadeRectangle; duration: 200; }
    }

    PropertyAction { target: expandPhoto; property: "visible"; value: false; }
    PropertyAction { target: fadeRectangle; property: "visible"; value: false; }

    onCompleted: {
      transitionToPhotoViewerCompleted();
    }
  }

  SequentialAnimation {
    id: hidePhotoViewerAnimation

    property variant thumbnailRect: {"x": 0, "y": 0, "width": 0, "height": 0}

    ParallelAnimation {
      FadeOutAnimation { target: fadeRectangle; duration: 200; }
      SequentialAnimation {
        PropertyAction { target: expandPhoto; property: "visible"; value: true; }
        ExpandAnimation {
          target: expandPhoto
          endX: hidePhotoViewerAnimation.thumbnailRect.x
          endY: hidePhotoViewerAnimation.thumbnailRect.y
          endWidth: hidePhotoViewerAnimation.thumbnailRect.width
          endHeight: hidePhotoViewerAnimation.thumbnailRect.height
          duration: 160
          easingType: Easing.OutQuad
        }
        FadeOutAnimation { target: expandPhoto; duration: 40; }
      }
    }

    onCompleted: {
      transitionFromPhotoViewerCompleted();
    }
  }
}

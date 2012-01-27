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

// Some custom components and animations that we want to invoke whenever we
// bring up the photo viewer.
Item {
  id: photoViewerTransition
  objectName: "photoViewerTransition"

  x: 0
  y: 0
  width: parent.width
  height: parent.height

  function transitionToPhotoViewer(photo, activatedRect) {
    expandAnimationPhoto.prepareForAnimation(photo, activatedRect);
    switchToPhotoViewAnimation.start();
  }

  signal transitionCompleted()

  Rectangle {
    id: fadeInRectangle
    visible: false
    color: "#444444"
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    z: 1
  }

  PhotoComponent {
    id: expandAnimationPhoto
    visible: false
    isCropped: false
    isPreview: true
    isAnimate: true
    color: "#00000000"
    z: 1

    function prepareForAnimation(photo, activatedRect) {
      mediaSource = photo;

      x = activatedRect.x;
      y = activatedRect.y;
      width = activatedRect.width;
      height = activatedRect.height;

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
    id: switchToPhotoViewAnimation

    ParallelAnimation {
      SequentialAnimation {
        FadeInAnimation { target: expandAnimationPhoto; duration: 40; }
        ExpandAnimation { target: expandAnimationPhoto; duration: 160; }
      }
      FadeInAnimation { target: fadeInRectangle; duration: 200; }
    }

    PropertyAction { target: expandAnimationPhoto; property: "visible"; value: false; }
    PropertyAction { target: fadeInRectangle; property: "visible"; value: false; }

    onCompleted: {
      transitionCompleted();
    }
  }
}

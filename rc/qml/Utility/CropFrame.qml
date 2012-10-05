/*
 * Copyright (C) 2012 Canonical Ltd
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

// A semi-transparent black box with "frame"-like corners.  Meant to be used as
// the area around the crop window.
Rectangle {
  id: cropFrame

  signal resizedX(bool left, real dx)
  signal resizedY(bool top, real dy)

  color: "black"
  opacity: 0.5

  border.width: gu(2) // Half of which is obscured by the crop window.
  border.color: color

  // Top-left corner.
  CropCorner {
    isLeft: true
    isTop: true
    color: cropFrame.color
    onDragged: {
      cropFrame.resizedX(isLeft, dx);
      cropFrame.resizedY(isTop, dy);
    }
  }

  // Top-right corner.
  CropCorner {
    isLeft: false
    isTop: true
    color: cropFrame.color
    onDragged: {
      cropFrame.resizedX(isLeft, dx);
      cropFrame.resizedY(isTop, dy);
    }
  }

  // Bottom-left corner.
  CropCorner {
    isLeft: true
    isTop: false
    color: cropFrame.color
    onDragged: {
      cropFrame.resizedX(isLeft, dx);
      cropFrame.resizedY(isTop, dy);
    }
  }

  // Bottom-right corner.
  CropCorner {
    isLeft: false
    isTop: false
    color: cropFrame.color
    onDragged: {
      cropFrame.resizedX(isLeft, dx);
      cropFrame.resizedY(isTop, dy);
    }
  }

  // Left drag bar.
  CropDragArea {
    x: -gu(2)
    width: gu(3)
    anchors.verticalCenter: parent.center
    height: parent.height - gu(2)
    onDragged: cropFrame.resizedX(true, dx)
  }

  // Top drag bar.
  CropDragArea {
    y: -gu(2)
    height: gu(3)
    anchors.horizontalCenter: parent.center
    width: parent.width - gu(2)
    onDragged: cropFrame.resizedY(true, dy)
  }

  // Right drag bar.
  CropDragArea {
    x: parent.width - gu(1)
    width: gu(3)
    anchors.verticalCenter: parent.center
    height: parent.height - gu(2)
    onDragged: cropFrame.resizedX(false, dx)
  }

  // Bottom drag bar.
  CropDragArea {
    y: parent.height - gu(1)
    height: gu(3)
    anchors.horizontalCenter: parent.center
    width: parent.width - gu(2)
    onDragged: cropFrame.resizedY(false, dy)
  }
}

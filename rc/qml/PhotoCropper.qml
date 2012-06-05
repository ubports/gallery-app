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

import QtQuick 1.1
import Gallery 1.0

// Lets you interactively crop Photos from inside the PhotoEditor.
Item {
  id: photoCropper

  // Note: each element of the cropped rect will be in the range [0,1], since
  // in the UI we aren't using direct photo pixel values.
  signal cropped(variant rect)
  signal canceled()

  property variant photo

  function resetCropRegion() {
    cropRegion.reset();
  }

  PhotoComponent {
    id: original

    anchors.top: topToolbar.bottom
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right

    color: "black"

    mediaSource: photoCropper.photo
    ownerName: "PhotoCropper"

    // An item placed over the actual photo.
    Item {
      id: photoRegion

      x: (original.width - original.paintedWidth) / 2
      y: (original.height - original.paintedHeight) / 2
      width: original.paintedWidth
      height: original.paintedHeight

      onXChanged: cropRegion.reset()
      onYChanged: cropRegion.reset()
      onWidthChanged: cropRegion.reset()
      onHeightChanged: cropRegion.reset()

      Rectangle {
        id: tint

        anchors.fill: parent
        color: "white"
        opacity: 0.5
      }

      // An item defining the size of the crop rectangle.
      Item {
        id: cropRegion

        function reset() {
          x = Math.floor(photoRegion.width / 4)
          y = Math.floor(photoRegion.height / 4)
          width = Math.ceil(photoRegion.width / 2)
          height = Math.ceil(photoRegion.height / 2)
        }

        Component.onCompleted: reset()

        // TODO: also support pinch/splay resizing of the cropRegion.

        CropFrame {
          id: frame

          property real minSize: gu(4)

          anchors.fill: parent

          // This handles resizing in both dimensions.  first is whether we're
          // resizing the "first" edge, e.g. left or top (in which case we
          // adjust both position and span) vs. right or bottom (where we just
          // adjust the span).  position should be either "x" or "y", and span
          // is either "width" or "height".  This is a little complicated, and
          // coule probably be optimized with a little more thought.
          function resizeCropRegion(first, delta, position, span) {
            if (first) {
              // Left/top side.
              if (cropRegion[position] + delta < 0)
                delta = -cropRegion[position];
              if (cropRegion[span] - delta < minSize)
                delta = cropRegion[span] - minSize;

              cropRegion[position] += delta;
              cropRegion[span] -= delta;
            } else {
              // Right/bottom side.
              if (cropRegion[span] + delta < minSize)
                delta = minSize - cropRegion[span];
              if (cropRegion[position] + cropRegion[span] + delta > photoRegion[span])
                delta = photoRegion[span] - cropRegion[position] - cropRegion[span];

              cropRegion[span] += delta;
            }
          }

          onResizedX: resizeCropRegion(left, dx, "x", "width")
          onResizedY: resizeCropRegion(top, dy, "y", "height")
        }

        // A clipping region to display a non-tinted slice of the photo inside
        // the crop rectangle.
        Item {
          id: window

          anchors.fill: parent
          clip: true

          PhotoComponent {
            x: -cropRegion.x
            y: -cropRegion.y
            width: photoRegion.width
            height: photoRegion.height

            mediaSource: photoCropper.photo
            ownerName: "PhotoCropper"
          }
        }

        MouseArea {
          anchors.fill: parent
          anchors.margins: gu(1)

          drag.target: cropRegion
          drag.axis: Drag.XandYAxis
          drag.minimumX: 0
          drag.maximumX: photoRegion.width - cropRegion.width
          drag.minimumY: 0
          drag.maximumY: photoRegion.height - cropRegion.height
        }
      }
    }
  }

  Toolbar {
    id: topToolbar

    anchors.top: parent.top

    Text {
      anchors.centerIn: parent
      color: "#747273"
      text: "Crop"
    }

    ToolbarTextButton {
      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      anchors.leftMargin: gu(2)

      text: "Cancel"

      onPressed: photoCropper.canceled()
    }

    ToolbarTextButton {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      anchors.rightMargin: gu(2)

      text: "OK"

      onPressed: {
        // Need to translate to [0,1] since we aren't using photo pixel
        // coordinates here.
        var rect = Qt.rect(
          cropRegion.x / photoRegion.width,
          cropRegion.y / photoRegion.height,
          cropRegion.width / photoRegion.width,
          cropRegion.height / photoRegion.height);

        photoCropper.cropped(rect);
      }
    }
  }
}

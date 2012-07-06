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
import "../Capetown"

// Lets you interactively crop Photos from inside the PhotoEditor.
Item {
  id: photoCropper

  // Note: each element of the cropped rect will be in the range [0,1], since
  // in the UI we aren't using direct photo pixel values.
  signal cropped(variant rect)
  signal canceled()

  // readonly
  property variant photo // Set with enterCropper().

  // internal
  property variant ratio_crop_rect

  function enterCropper(photo) {
    var ratio_crop_rect = photo.prepareForCropping();

    photoCropper.ratio_crop_rect = ratio_crop_rect;
    photoCropper.photo = photo;
  }

  MouseArea {
    id: cancelArea

    anchors.fill: parent

    onPressed: {
      photo.revertToLastSavePoint();
      photoCropper.canceled()
    }
  }

  PhotoComponent {
    id: original

    anchors.fill: parent
    anchors.topMargin: gu(6) /* toolbar height */ + gu(3)
    anchors.bottomMargin: gu(6) /* toolbar height */ + gu(3)
    anchors.leftMargin: gu(3)
    anchors.rightMargin: gu(3)

    color: "black"

    mediaSource: photoCropper.photo
    ownerName: "PhotoCropper"

    onLoaded: {
      // This is kind of silly, but I couldn't find any other way to get the
      // dimensions to be accurate when we need to reset the crop rectangle.
      cropRegion.reset(photoCropper.ratio_crop_rect);
    }

    // An item placed over the actual photo.
    Item {
      id: photoRegion

      x: (original.width - original.paintedWidth) / 2
      y: (original.height - original.paintedHeight) / 2
      width: original.paintedWidth
      height: original.paintedHeight

      Rectangle {
        id: tint

        anchors.fill: parent
        color: "white"
        opacity: 0.5
      }

      // An item defining the size of the crop rectangle.
      Item {
        id: cropRegion

        function reset(ratio_crop_rect) {
          x = Math.floor(ratio_crop_rect.x * photoRegion.width);
          y = Math.floor(ratio_crop_rect.y * photoRegion.height);
          width = Math.ceil(ratio_crop_rect.width * photoRegion.width);
          height = Math.ceil(ratio_crop_rect.height * photoRegion.height);
        }

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
          id: dragArea

          anchors.fill: parent
          anchors.margins: gu(1)

          drag.target: cropRegion
          drag.axis: Drag.XandYAxis
          drag.minimumX: 0
          drag.maximumX: photoRegion.width - cropRegion.width
          drag.minimumY: 0
          drag.maximumY: photoRegion.height - cropRegion.height
        }

        ToolbarIconButton {
          id: cropButton

          anchors.centerIn: parent

          selectedIconFilename: "../img/icon-crop.png"
          deselectedIconFilename: selectedIconFilename

          state: (dragArea.pressed ? "faded" : "visible")
          states: [
            State { name: "visible";
              PropertyChanges { target: cropButton; opacity: 1; }
            },
            State { name: "faded";
              PropertyChanges { target: cropButton; opacity: 0; }
            }
          ]

          Behavior on opacity {
            animation: NumberAnimation {
              easing.type: Easing.OutQuad
              duration: 200
            }
          }

          onPressed: {
            photo.discardLastSavePoint();

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
  }
}

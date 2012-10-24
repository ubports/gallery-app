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
import "../../js/GalleryUtility.js" as GalleryUtility
import "../../js/GraphicsRoutines.js" as GraphicsRoutines

// PhotoComponent that allows you to zoom in on the photo.
Rectangle {
  id: zoomablePhotoComponent

  signal loaded()
  signal clicked()
  signal zoomed()
  signal unzoomed()

  property var mediaSource
  property bool isPreview
  property string ownerName

  // read-only
  property alias paintedWidth: unzoomedPhoto.paintedWidth
  property alias paintedHeight: unzoomedPhoto.paintedHeight
  property alias isLoaded: unzoomedPhoto.isLoaded
  property int zoomFocusX: 0 // Relative to zoomablePhotoComponent.
  property int zoomFocusY: 0
  property real zoomFactor: 1
  property bool fullyUnzoomed: (state === "unzoomed" && zoomFactor === 1)
  property bool fullyZoomed: (state === "full_zoom" && zoomFactor === maxZoomFactor)

  // internal
  property real maxZoomFactor: 2.5
  property real photoFocusX: zoomFocusX - unzoomedPhoto.leftEdge
  property real photoFocusY: zoomFocusY - unzoomedPhoto.topEdge

  clip: true

  function zoom(x, y) {
    zoomFocusX = x;
    zoomFocusY = y;
    state = "full_zoom";
  }

  function unzoom() {
    state = "unzoomed";
  }

  function toggleZoom(x, y) {
    if (state === "unzoomed")
      zoom(x, y);
    else
      unzoom();
  }

  states: [
    State { name: "unzoomed";
      PropertyChanges { target: zoomablePhotoComponent; zoomFactor: 1; } },

    State { name: "full_zoom";
      PropertyChanges { target: zoomablePhotoComponent; zoomFactor: maxZoomFactor; } }
  ]

  transitions: [
    Transition { from: "*"; to: "unzoomed";
      NumberAnimation { properties: "zoomFactor"; easing.type: Easing.InQuad;
                        duration: 350; } },
    Transition { from: "*"; to: "full_zoom";
      NumberAnimation { properties: "zoomFactor"; easing.type: Easing.InQuad;
                        duration: 350; } }
  ]

  state: "unzoomed"

  onStateChanged: {
    if (state === "zoomed")
      zoomed();
    else
      unzoomed();
  }

  GalleryPhotoComponent {
    id: unzoomedPhoto

    property real leftEdge: (parent.width - paintedWidth) / 2
    property real topEdge: (parent.height - paintedHeight) / 2

    anchors.fill: parent
    visible: fullyUnzoomed
    color: zoomablePhotoComponent.color

    mediaSource: zoomablePhotoComponent.mediaSource
    isPreview: zoomablePhotoComponent.isPreview
    ownerName: zoomablePhotoComponent.ownerName
  }

  MouseArea {
    anchors.fill: parent
    enabled: fullyUnzoomed

    onClicked: zoomablePhotoComponent.clicked()
    onDoubleClicked: {
      if (mouse.x >= unzoomedPhoto.leftEdge &&
          mouse.x < unzoomedPhoto.leftEdge + unzoomedPhoto.paintedWidth &&
          mouse.y >= unzoomedPhoto.topEdge &&
          mouse.y < unzoomedPhoto.topEdge + unzoomedPhoto.paintedHeight)
        zoom(mouse.x, mouse.y);
      else
        zoomablePhotoComponent.clicked();
    }
  }

  Flickable {
    id: zoomArea

    property real zoomAreaZoomFactor: maxZoomFactor
    property real minContentFocusX: (contentWidth < parent.width
                                     ? contentWidth : parent.width) / 2
    property real maxContentFocusX: contentWidth - minContentFocusX
    property real minContentFocusY: (contentHeight < parent.height
                                     ? contentHeight : parent.height) / 2
    property real maxContentFocusY: contentHeight - minContentFocusY
    property real contentFocusX: GraphicsRoutines.clamp(
                                   photoFocusX * zoomAreaZoomFactor,
                                   minContentFocusX, maxContentFocusX)
    property real contentFocusY: GraphicsRoutines.clamp(
                                   photoFocusY * zoomAreaZoomFactor,
                                   minContentFocusY, maxContentFocusY)
    // Translate between focus point and top/left point.  Note: you might think
    // that this should take into account the left and top margins, but
    // apparently not.
    property real contentFocusLeft: contentFocusX - parent.width / 2
    property real contentFocusTop: contentFocusY - parent.height / 2

    anchors.fill: parent
    visible: fullyZoomed

    onVisibleChanged: {
      if (visible) {
        contentX = contentFocusLeft;
        contentY = contentFocusTop;
      }
    }

    onContentXChanged: {
      var contentFocusX = contentX + parent.width / 2;
      var photoFocusX = contentFocusX / zoomAreaZoomFactor;
      zoomFocusX = photoFocusX + unzoomedPhoto.leftEdge;
    }
    onContentYChanged: {
      var contentFocusY = contentY + parent.height / 2;
      var photoFocusY = contentFocusY / zoomAreaZoomFactor;
      zoomFocusY = photoFocusY + unzoomedPhoto.topEdge;
    }

    flickableDirection: Flickable.HorizontalAndVerticalFlick
    contentWidth: unzoomedPhoto.paintedWidth * zoomAreaZoomFactor
    contentHeight: unzoomedPhoto.paintedHeight * zoomAreaZoomFactor

    leftMargin: Math.max(0, (parent.width - contentWidth) / 2)
    rightMargin: leftMargin
    topMargin: Math.max(0, (parent.height - contentHeight) / 2)
    bottomMargin: topMargin

    GalleryPhotoComponent {
      id: zoomedPhoto

      anchors.fill: parent
      color: zoomablePhotoComponent.color

      // TODO: load in the background but only for the photo being viewed.
      mediaSource: (visible ? zoomablePhotoComponent.mediaSource : "")
      isPreview: zoomablePhotoComponent.isPreview
      ownerName: zoomablePhotoComponent.ownerName

      MouseArea {
        anchors.fill: parent

        onClicked: zoomablePhotoComponent.clicked()
        onDoubleClicked: unzoom()
      }
    }
  }

  GalleryPhotoComponent {
    id: transitionPhoto

    property real unzoomedX: unzoomedPhoto.leftEdge
    property real unzoomedY: unzoomedPhoto.topEdge
    property real zoomedX: -zoomArea.contentFocusLeft
    property real zoomedY: -zoomArea.contentFocusTop

    property real zoomFraction: (zoomFactor - 1) / (maxZoomFactor - 1)

    x: GalleryUtility.interpolate(unzoomedX, zoomedX, zoomFraction)
    y: GalleryUtility.interpolate(unzoomedY, zoomedY, zoomFraction)
    width: unzoomedPhoto.paintedWidth
    height: unzoomedPhoto.paintedHeight
    scale: zoomFactor
    transformOrigin: Item.TopLeft

    visible: (!fullyUnzoomed && !zoomedPhoto.isLoaded)
    color: zoomablePhotoComponent.color

    mediaSource: zoomablePhotoComponent.mediaSource
    isPreview: zoomablePhotoComponent.isPreview
    ownerName: zoomablePhotoComponent.ownerName
  }
}

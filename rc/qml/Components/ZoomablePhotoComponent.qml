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
  property bool load: false
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
  property bool zoomedOnce: false
  property var zoomArea
  property var transitionPhoto
  property var zoomedPhoto
  property bool isPinching: pinchArea.pinch.active

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
      PropertyChanges { target: zoomablePhotoComponent; zoomFactor: maxZoomFactor; } 
    },
    
    State { name: "pinch"; }
  ]

  transitions: [
    // Double-click transitions.
    Transition { from: "full_zoom"; to: "unzoomed";
      SequentialAnimation {
        ScriptAction { script: {
            transitionPhoto.visible = true;
            zoomArea.visible = false;
          }
        }
        NumberAnimation { properties: "zoomFactor"; easing.type: Easing.InQuad;
          duration: 350; }
        ScriptAction { script: transitionPhoto.visible = false; }
      }
    },
    Transition { from: "unzoomed"; to: "full_zoom";
      SequentialAnimation {
        ScriptAction { script: {
            transitionPhoto.visible = true;
          }
        }
        NumberAnimation { properties: "zoomFactor"; easing.type: Easing.InQuad;
          duration: 350; }
        ScriptAction { script: {
            zoomArea.placeZoomedImage();
            zoomArea.visible = true;
            if (zoomedPhoto.isLoaded)
              transitionPhoto.visible = false; 
          }
        }
      }
    },
    
    // Pinch transitions.
    Transition { from: "pinch"; to: "unzoomed";
      SequentialAnimation {
        NumberAnimation { properties: "zoomFactor"; easing.type: Easing.Linear;
          duration: 100; }
        ScriptAction { script: transitionPhoto.visible = false; }
      }
    },
    Transition { from: "pinch"; to: "full_zoom";
      SequentialAnimation {
        NumberAnimation { properties: "zoomFactor"; easing.type: Easing.Linear;
          duration: 100; }
        ScriptAction { script: {
            zoomArea.placeZoomedImage();
            zoomArea.visible = true;
            if (zoomedPhoto.isLoaded)
              transitionPhoto.visible = false; 
          }
        }
      }
    }
  ]

  state: "unzoomed"

  onStateChanged: {
    if (state === "full_zoom")
      zoomed();
    else if (state === "unzoomed")
      unzoomed();
  }

  onFullyZoomedChanged: {
    if (fullyZoomed && !zoomedOnce)
      zoomedOnce = true;
  }
  
  onFullyUnzoomedChanged: {
    if (zoomedOnce && fullyUnzoomed) {
      zoomAssemblyLoader.sourceComponent = undefined;
      zoomAssemblyLoader.sourceComponent = zoomAssemblyComponent;
    }
  }

  GalleryPhotoComponent {
    id: unzoomedPhoto

    property real leftEdge: (parent.width - paintedWidth) / 2
    property real topEdge: (parent.height - paintedHeight) / 2

    anchors.fill: parent
    visible: fullyUnzoomed
    color: zoomablePhotoComponent.color
    
    mediaSource: zoomablePhotoComponent.mediaSource
    load: zoomablePhotoComponent.load && zoomablePhotoComponent.fullyUnzoomed
    isPreview: zoomablePhotoComponent.isPreview
    ownerName: zoomablePhotoComponent.ownerName + "unzoomedPhoto"
  }
  
  PinchArea {
    id: pinchArea
    
    // True if we were zooming in previously, else false.
    property bool zoomIn: false
    
    // Set to true once zoom has started, then immediately set to false.
    property bool zoomStarted: false
    
    // Normalizes the pinch area's scale, which always starts at 1, to the
    // actual zoom factor used by the photo component.
    function pinchAreaScaleToZoomFactor(scaleFactor) {
      return zoomIn ? scaleFactor : scaleFactor * maxZoomFactor;
    }
    
    anchors.fill: parent
    enabled: true
    
    onPinchStarted: {
      zoomStarted = true;
      zoomIn = false;
    }
    
    onPinchUpdated: {
      // Determine if we're still zooming in or out.  Allow for a small
      // variance to account for touch noise.
      zoomIn = (pinch.scale > pinch.previousScale + (zoomIn ? -0.001 : 0.001));
      
      // Prevent zooming in on full photo or zooming out of unzoomed photo.
      if ((zoomIn && zoomFactor === maxZoomFactor) ||
          (!zoomIn && zoomFactor === 1.0))
        return;
      
      // Setup params after a successful pinch has begun.
      if (zoomStarted) {
        zoomablePhotoComponent.state = "pinch";
        transitionPhoto.visible = true;
        zoomArea.visible = false;
        zoomStarted = false;
        
        if (zoomIn) {
          zoomFocusX = pinch.center.x;
          zoomFocusY = pinch.center.y;
        }
      }
      
      // Set the zoom factor appropriately.
      // Don't allow user to scale all the way up 
      zoomFactor = GraphicsRoutines.clamp(pinchAreaScaleToZoomFactor(pinch.scale),
        1.0, maxZoomFactor - 0.01);
    }
    
    onPinchFinished: {
      // Fire a state change to complete the zoom.
      zoomablePhotoComponent.state = zoomIn ? "full_zoom" :
        zoomablePhotoComponent.state = "unzoomed";
    }
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

  Loader {
    id: zoomAssemblyLoader
        
    anchors.fill: parent
    
    sourceComponent: zoomAssemblyComponent
    
    onLoaded: {
      item.connectToContext();
    }
    
    Component {
      id: zoomAssemblyComponent
      
      Item {
        id: zoomAssembly
        
        function connectToContext() {
          zoomablePhotoComponent.zoomArea = zoomArea;
          zoomablePhotoComponent.transitionPhoto = transitionPhoto;
          zoomablePhotoComponent.zoomedPhoto = zoomedPhoto;
        }
        
        anchors.fill: parent

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
      
          function placeZoomedImage() {
            contentX = contentFocusLeft;
            contentY = contentFocusTop;
          }
          
          anchors.fill: parent
          visible: false
      
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
      
            mediaSource: zoomablePhotoComponent.mediaSource
            load: zoomablePhotoComponent.load && fullyZoomed
            
            isPreview: zoomablePhotoComponent.isPreview
            ownerName: zoomablePhotoComponent.ownerName + "zoomedPhoto"
            onIsLoadedChanged: {
              // Hide the transition when photo is loaded.
              if (isLoaded && fullyZoomed)
                transitionPhoto.visible = false;
            }
      
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
      
          visible: isPinching
          color: zoomablePhotoComponent.color
      
          mediaSource: zoomablePhotoComponent.mediaSource
          load: zoomablePhotoComponent.load && visible
          isPreview: zoomablePhotoComponent.isPreview
          isAnimate: true
          ownerName: zoomablePhotoComponent.ownerName + "transitionPhoto"
        }
      }
    }
  }
}

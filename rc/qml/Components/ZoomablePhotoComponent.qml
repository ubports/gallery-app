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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import "../../js/Gallery.js" as Gallery
import "../../js/GalleryUtility.js" as GalleryUtility
import "../../js/GraphicsRoutines.js" as GraphicsRoutines

// PhotoComponent that allows you to zoom in on the photo.
Rectangle {
    id: zoomablePhotoComponent

    /*!
    */
    signal loaded()
    /*!
    */
    signal clicked()
    /*!
    */
    signal zoomed()
    /*!
    */
    signal unzoomed()

    /*!
    */
    property var mediaSource
    /*!
    */
    property bool load: false
    /*!
    */
    property bool isPreview
    /*!
    */
    property string ownerName

    // read-only
    /*!
    */
    property alias paintedWidth: unzoomedPhoto.paintedWidth
    /*!
    */
    property alias paintedHeight: unzoomedPhoto.paintedHeight
    /*!
    */
    property alias isLoaded: unzoomedPhoto.isLoaded
    /*!
    */
    property int zoomFocusX: 0 // Relative to zoomablePhotoComponent.
    /*!
    */
    property int zoomFocusY: 0
    /*!
    */
    property real zoomFactor: 1
    /*!
    */
    property bool fullyUnzoomed: (state === "unzoomed" && zoomFactor === 1)
    /*!
    */
    property bool fullyZoomed: (state === "full_zoom" && zoomFactor === maxZoomFactor)

    // Though little documented, Qt has a dedicated background thread, separate
    // from the main GUI thread, in which it renders on-screen objects (see
    // http://blog.qt.digia.com/blog/2012/08/20/render-thread-animations-in-qt-quick-2-0/
    // for a discussion of this topic). Unfortunately, animation ticks are timed
    // by the main GUI thread, but actual drawing in response to these ticks
    // is done in the separate rendering thread. Because of this, you can get
    // into a situation in which an animation reports that it has completed but
    // the separate rendering thread still has a frame to draw. In all of my
    // testing, I've never seen this timing mismatch exceed 1/30th of a second,
    // which makes sense because the QML animation clock ticks every 1/60th of a
    // second, according to the docs (see http://qt-project.org/doc/qt-5.0/qtqml/qml-qtquick2-timer.html),
    // though implementations appear to be free to drop to half this rate
    // (see https://bugreports.qt-project.org/browse/QTBUG-28487). So we define
    // an animation frame as 1/30th of a second and wait this long before doing
    // more drawing in response to the completion of an animation to prevent
    // stuttering.
    property int oneFrame: Math.ceil(1000 / 30);

    // internal
    /*!
    */
    property real maxZoomFactor: 2.5
    /*!
    */
    property real photoFocusX: zoomFocusX - unzoomedPhoto.leftEdge
    /*!
    */
    property real photoFocusY: zoomFocusY - unzoomedPhoto.topEdge
    /*!
    */
    property bool isZoomAnimationInProgress: false

    clip: true

    /*!
    */
    function zoom(x, y) {
        zoomFocusX = x;
        zoomFocusY = y;
        state = "full_zoom";
    }

    /*!
    */
    function unzoom() {
        state = "unzoomed";
    }

    /*!
    */
    function toggleZoom(x, y) {
        if (state === "unzoomed")
            zoom(x, y);
        else
            unzoom();
    }

    states: [
        State { name: "unzoomed";
            PropertyChanges { target: zoomablePhotoComponent; zoomFactor: 1; }
        },
        State { name: "full_zoom";
            PropertyChanges { target: zoomablePhotoComponent; zoomFactor: maxZoomFactor; }
        },
        State { name: "pinching";
            // Setting the zoom factor to itself seems odd, but it's necessary to
            // prevent zoomFactor from jumping when you start pinching.
            PropertyChanges { target: zoomablePhotoComponent; zoomFactor: zoomFactor;
                explicit: true; }
        }
    ]

    transitions: [
        // Double-click transitions.
        Transition { from: "full_zoom"; to: "unzoomed";
            SequentialAnimation {
                ScriptAction { script: isZoomAnimationInProgress = true; }
                NumberAnimation { properties: "zoomFactor"; easing.type: Easing.InQuad;
                    duration: Gallery.FAST_DURATION; }
                PauseAnimation { duration: oneFrame }
                ScriptAction { script: isZoomAnimationInProgress = false; }
            }
        },

        Transition { from: "unzoomed"; to: "full_zoom";
            SequentialAnimation {
                ScriptAction { script: isZoomAnimationInProgress = true; }
                NumberAnimation { properties: "zoomFactor"; easing.type: Easing.InQuad;
                    duration: Gallery.FAST_DURATION; }
                PauseAnimation { duration: oneFrame }
                ScriptAction { script: isZoomAnimationInProgress = false; }
            }
        },

        Transition { from: "pinching"; to: "unzoomed";
            SequentialAnimation {
                ScriptAction { script: isZoomAnimationInProgress = true; }
                NumberAnimation { properties: "zoomFactor"; easing.type: Easing.Linear;
                    duration: Gallery.SNAP_DURATION; }
                PauseAnimation { duration: oneFrame }
                ScriptAction { script: isZoomAnimationInProgress = false; }
            }
        },

        Transition { from: "pinching"; to: "full_zoom";
            SequentialAnimation {
                ScriptAction { script: isZoomAnimationInProgress = true; }
                NumberAnimation { properties: "zoomFactor"; easing.type: Easing.Linear;
                    duration: Gallery.SNAP_DURATION; }
                PauseAnimation { duration: oneFrame }
                ScriptAction { script: isZoomAnimationInProgress = false; }
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

    GalleryPhotoComponent {
        id: unzoomedPhoto

        property real leftEdge: (parent.width - paintedWidth) / 2
        property real topEdge: (parent.height - paintedHeight) / 2

        function isInsidePhoto(x, y) {
            return (x >= leftEdge && x < leftEdge + paintedWidth &&
                    y >= topEdge && y < topEdge + paintedHeight);
        }

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

        property bool zoomingIn // Splaying to zoom in, vs. pinching to zoom out.
        property real initialZoomFactor

        anchors.fill: parent

        // QML seems to ignore these, so we have to manually keep scale in check
        // inside onPinchUpdated.  The 0.9 and 1.1 are just fudge factors to give
        // us a little bounce when you go past the zoom limit.
        pinch.minimumScale: 1 / initialZoomFactor * 0.9
        pinch.maximumScale: maxZoomFactor / initialZoomFactor * 1.1

        onPinchStarted: {
            zoomingIn = false;
            initialZoomFactor = zoomFactor;

            if (fullyUnzoomed) {
                if (unzoomedPhoto.isInsidePhoto(pinch.center.x, pinch.center.y)) {
                    zoomFocusX = pinch.center.x;
                    zoomFocusY = pinch.center.y;
                } else {
                    zoomFocusX = parent.width / 2;
                    zoomFocusY = parent.height / 2;
                }
            }

            zoomablePhotoComponent.state = "pinching";
        }

        onPinchUpdated: {
            // Determine if we're still zooming in or out.  Allow for a small
            // variance to account for touch noise.
            if (Math.abs(pinch.scale - pinch.previousScale) > 0.001)
                zoomingIn = (pinch.scale > pinch.previousScale);

            // For some reason, the PinchArea ignores these settings.
            var scale = GraphicsRoutines.clamp(pinch.scale,
                                               pinchArea.pinch.minimumScale, pinchArea.pinch.maximumScale);

            zoomFactor = initialZoomFactor * scale;
        }

        onPinchFinished: zoomablePhotoComponent.state = (zoomingIn ? "full_zoom" : "unzoomed")

        MouseArea {
            anchors.fill: parent
            enabled: fullyUnzoomed

            onClicked: zoomablePhotoComponent.clicked()
            onDoubleClicked: {
                if (unzoomedPhoto.isInsidePhoto(mouse.x, mouse.y))
                    zoom(mouse.x, mouse.y);
                else
                    zoomablePhotoComponent.clicked();
            }
        }
    }

    Loader {
        id: zoomAssemblyLoader

        anchors.fill: parent

        sourceComponent: (fullyUnzoomed ? undefined : zoomAssemblyComponent)

        Component {
            id: zoomAssemblyComponent

            Item {
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

                    anchors.fill: parent
                    visible: fullyZoomed && !isZoomAnimationInProgress

                    onVisibleChanged: {
                        if (visible) {
                            contentX = contentFocusLeft;
                            contentY = contentFocusTop;
                        }
                    }

                    onContentXChanged: {
                        var contentFocusX = contentX + width / 2;
                        var photoFocusX = contentFocusX / zoomAreaZoomFactor;
                        zoomFocusX = photoFocusX + unzoomedPhoto.leftEdge;
                    }

                    onContentYChanged: {
                        var contentFocusY = contentY + height / 2;
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

                    visible: zoomablePhotoComponent.isZoomAnimationInProgress ||
                             zoomablePhotoComponent.state == "pinching"

                    color: zoomablePhotoComponent.color

                    mediaSource: zoomablePhotoComponent.mediaSource
                    load: zoomablePhotoComponent.load
                    isPreview: zoomablePhotoComponent.isPreview
                    isAnimate: true
                    ownerName: zoomablePhotoComponent.ownerName + "transitionPhoto"
                }
            }
        }
    }
}

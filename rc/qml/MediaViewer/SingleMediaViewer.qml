/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtMultimedia 5.0
import Ubuntu.Components 1.0
import Ubuntu.Components.ListItems 1.0 as ListItems
import Ubuntu.Components.Popups 1.0
import Ubuntu.Content 0.1
import Ubuntu.Thumbnailer 0.1
import Gallery 1.0
import "../Components"

Item {
    id: viewer
    property bool pinchInProgress: zoomPinchArea.active
    property var mediaSource
    property real maxDimension: 0.
    property bool showThumbnail: true

    property bool isVideo: mediaSource.type === MediaSource.Video
    property bool userInteracting: pinchInProgress || flickable.sizeScale != 1.0
    property bool fullyZoomed: flickable.sizeScale == zoomPinchArea.maximumZoom
    property bool fullyUnzoomed: flickable.sizeScale == zoomPinchArea.minimumZoom
    property bool animateMediaOnHeight: false
    property bool imageReady: image.status == Image.Ready

    property alias paintedHeight: image.paintedHeight
    property alias paintedWidth: image.paintedWidth

    signal clicked()

    onHeightChanged: {
        if (height > viewer.maxDimension)
            viewer.maxDimension = height;
    }

    onWidthChanged: {
        if (width > viewer.maxDimension)
            viewer.maxDimension = width;
    }

    function zoomIn(centerX, centerY, factor) {
        flickable.scaleCenterX = centerX / (flickable.sizeScale * flickable.width);
        flickable.scaleCenterY = centerY / (flickable.sizeScale * flickable.height);
        flickable.sizeScale = factor;
    }

    function zoomOut() {
        if (flickable.sizeScale != 1.0) {
            flickable.scaleCenterX = flickable.contentX / flickable.width / (flickable.sizeScale - 1);
            flickable.scaleCenterY = flickable.contentY / flickable.height / (flickable.sizeScale - 1);
            flickable.sizeScale = 1.0;
        }
    }

    function reset() {
        if (!viewer.isVideo)
            zoomOut()
    }

    ActivityIndicator {
        anchors.centerIn: parent
        visible: running
        running: image.status != Image.Ready
    }

    PinchArea {
        id: zoomPinchArea
        anchors.fill: parent

        property real initialZoom
        property real maximumScale: 3.0
        property real minimumZoom: 1.0
        property real maximumZoom: 3.0
        property bool active: false
        property var center

        onPinchStarted: {
            active = true;
            initialZoom = flickable.sizeScale;
            center = zoomPinchArea.mapToItem(media, pinch.startCenter.x, pinch.startCenter.y);
            zoomIn(center.x, center.y, initialZoom);
        }
        onPinchUpdated: {
            var zoomFactor = MathUtils.clamp(initialZoom * pinch.scale, minimumZoom, maximumZoom);
            flickable.sizeScale = zoomFactor;
        }
        onPinchFinished: {
            active = false;
        }

        Flickable {
            id: flickable
            anchors.fill: parent
            contentWidth: media.width
            contentHeight: media.height
            contentX: (sizeScale - 1) * scaleCenterX * width
            contentY: (sizeScale - 1) * scaleCenterY * height
            interactive: !viewer.pinchInProgress

            property real sizeScale: 1.0
            property real scaleCenterX: 0.0
            property real scaleCenterY: 0.0
            Behavior on sizeScale {
                enabled: !viewer.pinchInProgress
                UbuntuNumberAnimation {duration: UbuntuAnimation.FastDuration}
            }
            Behavior on scaleCenterX {
                UbuntuNumberAnimation {duration: UbuntuAnimation.FastDuration}
            }
            Behavior on scaleCenterY {
                UbuntuNumberAnimation {duration: UbuntuAnimation.FastDuration}
            }

            Item {
                id: media

                width: flickable.width * flickable.sizeScale
                height: flickable.height * flickable.sizeScale

                Connections {
                    target: mediaSource
                    onDataChanged: {
                        image.source = "";
                        image.source = "image://photo/" + mediaSource.path

                        rightResolutionImage.source = "";
                        rightResolutionImage.source = "image://photo/" + mediaSource.path
                    }
                }

                Behavior on height {
                    enabled: viewer.animateMediaOnHeight 
                    UbuntuNumberAnimation {}
                }

                Image {
                    id: image
                    anchors.fill: parent
                    asynchronous: true
                    cache: false
                    source: viewer.isVideo ? "image://thumbnailer/" + mediaSource.path : "image://photo/" + mediaSource.path
                    sourceSize {
                        width: viewer.maxDimension
                        height: viewer.maxDimension
                    }
                    fillMode: Image.PreserveAspectFit
                    visible: viewer.showThumbnail
                    opacity: status == Image.Ready ? 1.0 : 0.0
                    Behavior on opacity { UbuntuNumberAnimation {duration: UbuntuAnimation.FastDuration} }

                }

                Image {
                    id: highResolutionImage
                    anchors.fill: parent
                    asynchronous: true
                    cache: false
                    // Load image using the photo image provider to ensure EXIF orientation
                    source: flickable.sizeScale > 1.0 ? "image://photo/" + mediaSource.path : ""
                    sourceSize {
                        width: width
                        height: height
                    }
                    opacity: status == Image.Ready ? 1.0 : 0.0
                    fillMode: Image.PreserveAspectFit
                }
            }

            Icon {
                width: units.gu(5)
                height: units.gu(5)
                anchors.centerIn: parent
                name: "media-playback-start"
                color: "white"
                opacity: 0.8
                visible: viewer.isVideo
            }

            MouseArea {
                id: viewerMouseArea
                anchors.fill: parent
                property bool eventAccepted: false

                onDoubleClicked: {
                    if (viewerMouseArea.eventAccepted)
                        return;

                    clickTimer.stop();
                    viewer.animateMediaOnHeight = false

                    if (viewer.ListView.view.moving) {
                        // FIXME: workaround for Qt bug specific to touch:
                        // doubleClicked is received even though the MouseArea
                        // was tapped only once but another MouseArea was also
                        // tapped shortly before.
                        // Ref.: https://bugreports.qt.io/browse/QTBUG-39332
                        return;
                    }
                    if (viewer.isVideo) return;

                    if (flickable.sizeScale < zoomPinchArea.maximumZoom) {
                        zoomIn(mouse.x, mouse.y, zoomPinchArea.maximumZoom);
                    } else {
                        zoomOut();
                    }
                }
                onClicked: {
                    viewerMouseArea.eventAccepted = false
                    clickTimer.start()
                }

                Timer {
                    id: clickTimer
                    interval: 200 
                    onTriggered: {
                        viewerMouseArea.eventAccepted = true
                        viewer.animateMediaOnHeight = true
                        viewer.clicked()
                    }
                }
            }

            MouseArea {
                anchors.centerIn: parent
                width: units.gu(10)
                height: units.gu(10)
                enabled: viewer.isVideo
                onClicked: {
                    if (viewer.isVideo) {
                        var url = mediaSource.path.toString().replace("file://", "video://");
                        Qt.openUrlExternally(url);
                    }
                }
            }
        }
    }
}

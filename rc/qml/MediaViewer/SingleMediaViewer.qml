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
    property real maxDimension
    property bool showThumbnail: true

    property bool isVideo: mediaSource.type === MediaSource.Video
    property bool isPlayingVideo: isVideo && video.isPlaying
    property bool userInteracting: pinchInProgress || flickable.sizeScale != 1.0
    property bool fullyZoomed: flickable.sizeScale == zoomPinchArea.maximumZoom
    property bool fullyUnzoomed: flickable.sizeScale == zoomPinchArea.minimumZoom

    property alias paintedHeight: image.paintedHeight
    property alias paintedWidth: image.paintedWidth

    signal clicked()

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

    function reload() {
        if (!viewer.isVideo) {
            var src = image.source
            image.asynchronous = false
            image.source = ""
            image.asynchronous = true
            image.source = src;

            src = highResolutionImage.source
            highResolutionImage.asynchronous = false
            highResolutionImage.source = ""
            highResolutionImage.asynchronous = true
            highResolutionImage.source = src
        }
    }

    function reset() {
        if (viewer.isVideo) {
            if (video.item) {
                video.item.stop();
                video.sourceComponent = null;
            }
        } else zoomOut()
    }

    function togglePlayPause() {
        if (video.isPlaying) video.pause();
        else video.play();
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

               Image {
                    id: image
                    anchors.fill: parent
                    asynchronous: true
                    cache: false
                    source: "image://thumbnailer/" + mediaSource.path
                    sourceSize {
                        width: viewer.maxDimension
                        height: viewer.maxDimension
                    }
                    fillMode: Image.PreserveAspectFit
                    visible: viewer.showThumbnail && video.status !== Loader.Ready
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
                    fillMode: Image.PreserveAspectFit
                }
            }

            Loader {
                id: video
                anchors.fill: parent
                visible: viewer.isVideo && video.status == Loader.Ready &&
                         video.item.playbackState !== MediaPlayer.StoppedState
                onLoaded: {
                    item.source = mediaSource.path;
                    item.play()
                }

                property bool isPlaying: item && item.playbackState === MediaPlayer.PlayingState
                function play() {
                    if (item) {
                        item.play();
                    } else {
                        viewer.showThumbnail = false;
                        sourceComponent = component_video;
                    }
                }
                function pause() {
                    if (item) item.pause();
                }
            }

            Icon {
                width: units.gu(5)
                height: units.gu(5)
                anchors.centerIn: parent
                name: "media-playback-start"
                color: "white"
                opacity: 0.8
                visible: viewer.isVideo &&
                         (!video.item || item.playbackState === MediaPlayer.StoppedState)
            }

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    clickTimer.stop();
                    if (viewer.isVideo) return;

                    if (flickable.sizeScale < zoomPinchArea.maximumZoom) {
                        zoomIn(mouse.x, mouse.y, zoomPinchArea.maximumZoom);
                    } else {
                        zoomOut();
                    }
                }
                onClicked: clickTimer.start()

                Timer {
                    id: clickTimer
                    interval: 20
                    onTriggered: viewer.clicked()
                }
            }

            MouseArea {
                anchors.centerIn: parent
                width: units.gu(10)
                height: units.gu(10)
                enabled: viewer.isVideo
                onClicked: viewer.togglePlayPause()
            }
        }

        Component {
            id: component_video
            Video { }
        }
    }
}


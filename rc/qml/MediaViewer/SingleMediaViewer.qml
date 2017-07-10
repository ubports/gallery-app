/*
 * Copyright 2014-2015 Canonical Ltd.
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

import QtQuick 2.4
import QtMultimedia 5.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Ubuntu.Components.Popups 1.3
import Ubuntu.Content 1.3
import Ubuntu.Thumbnailer 0.1
import Ubuntu.Components.Extras 0.2
import Gallery 1.0
import "../Components"

Item {
    id: viewer
    property bool pinchInProgress: zoomPinchArea.active
    property var mediaSource
    property size thumbSize: Qt.size(viewer.width * 1.05, viewer.height * 1.05)
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

    onWidthChanged: {
        // Only change thumbSize if width increases more than 5%
        // that way we do not reload image for small resizes
        if (width > thumbSize.width) {
            thumbSize = Qt.size(width * 1.05, height * 1.05);
        }
    }

    onHeightChanged: {
        // Only change thumbSize if height increases more than 5%
        // that way we do not reload image for small resizes
        if (height > thumbSize.height) {
            thumbSize = Qt.size(width * 1.05, height * 1.05);
        }
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
        running: image.status != Image.Ready && image.status != Image.Error
    }

    PinchArea {
        id: zoomPinchArea
        anchors.fill: parent

        property real initialZoom
        property real maximumScale: 3.0
        property real minimumZoom: 1.0
        property real maximumZoom: 5.0
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

                        highResolutionImage.source = "";
                        highResolutionImage.source = "image://photo/" + mediaSource.path
                    }
                }

                Behavior on height {
                    enabled: viewer.animateMediaOnHeight 
                    UbuntuNumberAnimation {}
                }

                Image {
                    // Reduce width/hight so background is not visible on the border of camera photos
                    width: image.paintedWidth - 1
                    height: image.paintedHeight - 1
                    anchors.centerIn: image
                    visible: !viewer.isVideo && image.opacity == 1.0

                    asynchronous: true
                    cache: true
                    fillMode: Image.Tile
                    horizontalAlignment: Image.AlignLeft
                    verticalAlignment: Image.AlignTop
                    source: "../../img/transparency-bg.png"
                }

                Image {
                    id: image
                    anchors.fill: parent
                    asynchronous: true
                    cache: false
                    source: {
                        if (viewer.isVideo) {
                            return "image://thumbnailer/" + mediaSource.path
                        } else {
                            return "image://photo/" + mediaSource.path
                        }
                    }
                    sourceSize {
                        width: viewer.thumbSize.width
                        height: viewer.thumbSize.height
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

                Item {
                    id: mediaLoadingError
                    anchors.centerIn: parent
                    width: parent.width
                    height: mediaLoadingErrorIcon.height + units.gu(5) + mediaLoadingErrorLabel.contentHeight
                    visible: opacity > 0
                    opacity: image.status == Image.Error ? 1.0 : 0.0
                    Behavior on opacity { UbuntuNumberAnimation {duration: UbuntuAnimation.FastDuration} }     

                    Icon {
                        id: mediaLoadingErrorIcon
                        anchors.horizontalCenter: parent.horizontalCenter 
                        width: units.gu(8)
                        height: width
                        name: "stock_image"
                        color: "white"
                        opacity: 0.8
                    }

                    Label {
                        id: mediaLoadingErrorLabel
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            top: mediaLoadingErrorIcon.bottom
                            topMargin: units.gu(5)
                        }

                        width: units.gu(30)
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                        text: i18n.tr("An error has occurred attempting to load media")
                        fontSize: "large"
                        color: "lightgrey"
                    }
                }
            }

            Icon {
				// If the media is a video, show a 'play' icon
                width: units.gu(6)
                height: units.gu(6)
                anchors.centerIn: parent
                name: "media-playback-start"
                color: "white"
                opacity: 0.8
                visible: viewer.isVideo && image.status == Image.Ready
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
                enabled: viewer.isVideo && image.status == Image.Ready
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

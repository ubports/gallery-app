/*
 * Copyright (C) 2012-2015 Canonical Ltd
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

import QtQuick 2.4
import Gallery 1.0
import Ubuntu.Components 1.3
import Ubuntu.Thumbnailer 0.1
import "../Components"
import "../Utility"
import "../../js/Gallery.js" as Gallery
import "../../js/GalleryUtility.js" as GalleryUtility

/*!
An "organic" list of photos.  Used as the "tray" contents for each event in
the OrganicEventView, and the layout of the OrganicAlbumView.
*/
Item {
    id: organicMediaList
    objectName: "organicMediaList"

    ///
    signal pressed(var mediaSource, var thumbnailRect)

    ///
    property var event
    ///
    property alias mediaModel: eventView.model
    readonly property real organicMediaListHeight: __bigSize + __smallSize + __photosTopMargin + __margin + __margin/2
    readonly property alias mediaModelCount: mediaModel.count
    ///
    property SelectionState selection

    ///
    property int animationDuration: Gallery.FAST_DURATION
    ///
    property int animationEasingType: Easing.InQuint

    // readonly
    ///
    property int __mediaPerPattern: 6
    /// Size of the bigger thumbnails
    property var __bigSize: units.gu(19)
    /// Size of the smaller thumbnails
    property var __smallSize: units.gu(12)
    /// Space between the thumbnails
    property real __margin: units.gu(2)

    // internal, used to get the organix effect
    /// X-position shift for the delegates in one of the organic blocks
    property var __photoX: [-__margin, 0, 0, 0, 0, 0]
    /// Y-position shift for the delegates in one of the organic blocks
    property var __photoY: [__smallSize + __margin, 0, __bigSize + __margin,
        0, __bigSize + __margin, 0]
    /// Size of the delegate in one of the organic blocks
    property var __photoSize: [__bigSize,__smallSize, __smallSize,
        __bigSize, __smallSize, __smallSize]
    /// Size to be adapted for the organic effect
    property var __photoWidth: [__smallSize-__margin, __bigSize - (__smallSize + __margin),
        2 * __smallSize - __bigSize,__bigSize - (__smallSize + __margin),
        __smallSize, 0]
    /// Extra space on the right, for correct scroll boundary
    property var __footerWidth: [__smallSize, __bigSize - __smallSize,
        2 * __smallSize - __bigSize + __margin,
        __bigSize - __smallSize, __smallSize + __margin,
        0]

    /// Used to generate a spacing between the events
    property real __photosTopMargin: __margin / 2

    property alias mediaTypeFilter: mediaModel.mediaTypeFilter

    height: organicMediaListHeight

    function positionViewAtSelected(index) {
        eventView.positionViewAtIndex(index, ListView.Center);
    }

    Behavior on height {
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasingType
        }
    }

    Component {
        id: eventHeader
        Item {
            width: eventView.leftBuffer + 2*__margin //+ __smallSize
            height: __smallSize
            EventCard {
                x: eventView.leftBuffer + __margin
                y: __photosTopMargin
                width: __smallSize
                height: __smallSize

                visible: Boolean(event)

                event: organicMediaList.event

                OrganicItemInteraction {
                    selectionItem: event
                    selection: organicMediaList.selection
                    isEventHeader: true
                }
            }
        }
    }

    Component {
        id: thumbnailDelegate
        Item {
            objectName: "eventPhoto"
            property variant model
            property int index: 0
            property int patternPhoto: index % __mediaPerPattern
            property alias thumbnailLoaded: thumbnail.visible

            width: __photoWidth[patternPhoto]
            height: __photoSize[patternPhoto]

            UbuntuShape {
                id: thumbnail

                property bool isLoading: source.status === Image.Loading

                x: __photoX[patternPhoto]
                y: __photosTopMargin + __photoY[patternPhoto]
                width: parent.height
                height: parent.height

                Behavior on x {
                    NumberAnimation {
                        duration: animationDuration
                        easing.type: animationEasingType
                    }
                }
                Behavior on y {
                    NumberAnimation {
                        duration: animationDuration
                        easing.type: animationEasingType
                    }
                }
                Behavior on width {
                    NumberAnimation {
                        duration: animationDuration
                        easing.type: animationEasingType
                    }
                }
                Behavior on height {
                    NumberAnimation {
                        duration: animationDuration
                        easing.type: animationEasingType
                    }
                }

                visible: source.status === Image.Ready || source.status === Image.Error

                radius: "medium"

                backgroundColor: "black"
                sourceFillMode: UbuntuShape.PreserveAspectCrop
                source: Image {
                    id: thumbImage
                    source: "image://thumbnailer/" + model.mediaSource.path + "?at=" + model.mediaSource.lastModified
                    asynchronous: true

                    /* The SDK thumbnailer respects the freedesktop.org standard and uses 128 for the small
                     * thumbnail size, while the previous thumbnailer used 216. To maintain the same visual
                     * result as the previous thumbnailer, we force it to generate a large thumbnail, which
                     * is closer to the older one in size and looks identical when downscaled */
                    sourceSize {
                        // Make sure to request a thumbnail big enough to handle all the possible sizes
                        // that way media items will not request a new one every time it changes its
                        // position
                        width: __bigSize
                        height: __bigSize
                    }
                    fillMode: Image.PreserveAspectCrop
                }

                Icon {
					// Display a image icon if there is an error with the photo
                    anchors.centerIn: parent
                    width: units.gu(6)
                    height: width
                    visible: thumbImage.status == Image.Error
                    name: "stock_image"
                    color: "white"
                    opacity: 0.8
                }

                Icon {
				// Display a play icon if the thumbnail is from a video
					anchors.centerIn: parent
					width: units.gu(5)
					height: units.gu(5)
					name: "media-playback-start"
					color: "white"
					visible: model.mediaSource.type === MediaSource.Video && thumbImage.status == Image.Ready
				}

                OrganicItemInteraction {
                    objectName: "eventsViewPhoto"
                    selectionItem: model.mediaSource
                    selection: organicMediaList.selection

                    onPressed: {
                        var rect = GalleryUtility.getRectRelativeTo(thumbnail,
                                                                    organicMediaList);
                        organicMediaList.pressed(selectionItem, rect);
                    }
                }
            }

            Component {
                id: component_loadIndicator
                ActivityIndicator {
                    id: loadIndicator
                    running: true
                }
            }
            Loader {
                id: loader_loadIndicator
                anchors.centerIn: thumbnail
                sourceComponent: delayTimer.showIndicator && thumbnail.isLoading ?
                                     component_loadIndicator : undefined
                asynchronous: true

                Timer {
                    id: delayTimer
                    property bool showIndicator: false
                    interval: 35
                    onTriggered: showIndicator = true
                    Component.onCompleted: start()
                }
            }
        }
    }

    function positionViewAtBeginning() {
        eventView.positionViewAtBeginning();
    }

    ListView {
        id: eventView
        // the buffers are needed, as the listview does not draw items outside is visible area
        // but for the organic effect, x and width are "displaced" for some items (first, last)
        property int leftBuffer: __smallSize + __margin
        property int rightBuffer: __smallSize
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            leftMargin: -leftBuffer
            right: parent.right
            rightMargin: -rightBuffer
        }

        maximumFlickVelocity: units.gu(300)
        flickDeceleration: maximumFlickVelocity / 3
        cacheBuffer: width

        model: MediaCollectionModel {
            id: mediaModel
            forCollection: organicMediaList.event
            monitored: true
        }

        orientation: Qt.Horizontal

        header: eventHeader
        spacing: __margin
        delegate: Loader {
            id: thumbnailLoader
            objectName: "thumbnailLoader" + index

            property int patternPhoto: index % __mediaPerPattern

            sourceComponent: eventView.header.status == Component.Ready ? thumbnailDelegate : undefined
            asynchronous: true

            width: __photoWidth[patternPhoto]
            height: __photoSize[patternPhoto]

            Binding {
                target: thumbnailLoader.item
                property: "model"
                value: model
                when: thumbnailLoader.status == Loader.Ready
            }
            Binding {
                target: thumbnailLoader.item
                property: "index"
                value: index
                when: thumbnailLoader.status == Loader.Ready
            }
        }
        footer: Item {
            width: eventView.rightBuffer +
                   __footerWidth[mediaModel.count % __mediaPerPattern] +
                   __margin
        }

        // discplacement animations does not always work see
        // https://bugreports.qt-project.org/browse/QTBUG-29944
        add: Transition {
            NumberAnimation {
                properties: "opacity"
                from:0 ; to: 1
                duration: animationDuration
                easing.type: animationEasingType
            }
        }
        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: animationDuration
                easing.type: animationEasingType
            }
        }
    }
}

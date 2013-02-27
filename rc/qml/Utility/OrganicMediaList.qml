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
import Gallery 1.0
import Ubuntu.Components 0.1
import "../Components"
import "../../js/Gallery.js" as Gallery
import "../../js/GalleryUtility.js" as GalleryUtility

// An "organic" list of photos.  Used as the "tray" contents for each event in
// the OrganicEventView, and the layout of the OrganicAlbumView.
Item {
    id: organicMediaList
    objectName: "organicMediaList"

    /*!
    */
    signal pressed(var mediaSource, var thumbnailRect)

    /*!
    */
    property var event
    /*!
    */
    property alias mediaModel: eventView.model
    /*!
    */
    property SelectionState selection

    /*!
    */
    property int animationDuration: Gallery.FAST_DURATION
    /*!
    */
    property int animationEasingType: Easing.InQuint

    // readonly
    /*!
    */
    property int __mediaPerPattern: 6
    /*!
    */
    property var __bigSize: units.gu(19)
    /*!
    */
    property var __smallSize: units.gu(12)
    /*!
    */
    property real __margin: units.gu(2)

    // internal
    /*!
    */
    property var __photoX: [0, 0, __smallSize + __margin,
        (__bigSize + __margin), (__bigSize + __smallSize + __margin * 2), -(__bigSize+__margin)]
    /*!
    */
    property var __photoY: [0, __bigSize + __margin, __bigSize + __margin, 0, 0,
        __smallSize + __margin]
    /*!
    */
    property var __photoSize: [__bigSize, __smallSize, __smallSize, __smallSize, __smallSize,
        __bigSize]
    /*!
    */
    property var __photoWidth: [0, __smallSize + __margin, (2 * __smallSize + 2 * __margin) - (__bigSize + __margin),
        __smallSize + __margin, __smallSize + __margin, 0]
    /*!
    */
    property var __footerWidth: [0, __bigSize + __margin, __bigSize - __smallSize,
        __bigSize - __smallSize, 0, 0]
    /*!
    */
    property real __photosTopMargin: __margin / 2

    height: (mediaModel.count > 1) ?
                (__bigSize + __smallSize + __photosTopMargin + __margin + __margin/2) :
                (__bigSize + __photosTopMargin + __margin / 2)

    Behavior on height {
        NumberAnimation {
            duration: animationDuration
            easing.type: animationEasingType
        }
    }

    Component {
        id: eventHeader
        Item {
            width: eventView.leftBuffer + __margin + __smallSize
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
                }
            }
        }
    }

    Component {
        id: thumbnailDelegate
        Item {
            property int patternPhoto: index % __mediaPerPattern

            width: __photoWidth[patternPhoto]
            height: __photoSize[patternPhoto]

            UbuntuShape {
                id: thumbnail
                x: patternPhoto < 5 ? __margin : -__bigSize
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

                visible: image.status === Image.Ready

                radius: "medium"

                image: Image {
                    source: model.mediaSource.galleryThumbnailPath
                    asynchronous: true
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
                sourceComponent: delayTimer.showIndicator && !thumbnail.visible ?
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

    ListView {
        id: eventView
        // the buffers are needed, as the listview does not draw items outside is visible area
        // but for the organic effect, x and width are "displaced" for some items (first, last)
        property int leftBuffer: __bigSize + __margin
        property int rightBuffer: __bigSize
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
        cacheBuffer: 0

        model: MediaCollectionModel {
            id: mediaModel
            forCollection: organicMediaList.event
            monitored: true
        }

        orientation: Qt.Horizontal

        header: eventHeader
        delegate: thumbnailDelegate
        footer: Item {
            width: eventView.rightBuffer + __footerWidth[mediaModel.count % __mediaPerPattern]
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

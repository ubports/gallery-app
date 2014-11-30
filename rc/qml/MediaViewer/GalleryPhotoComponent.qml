/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0

// Basic photo component.  Can be used on its own, or as a delegate
// for PhotoViewer.
Rectangle {
    id: photoComponent

    /*!
    */
    property variant mediaSource
    /*!
    */
    property bool load: false

    /*!
    */
    property url source: {
        if (!load)
            return source;

        if (!mediaSource)
            return "";

        // TODO: If MediaSource could return dimensions of both original image
        // and its preview, could dynamically determine if the display dimensions
        // are smaller than preview's and automatically use that instead of a
        // full image load

        // Load image using the photo image provider to ensure EXIF orientation
        return "image://" + (isPreview ? "thumbnailer/" : "photo:/") + mediaSource.path;
    }

    /*!
    */
    property bool isCropped: false
    /*!
    */
    property bool isPreview: false
    /*!
    */
    property bool isZoomable: false
    /*!
    */
    property bool isAnimate: false
    /*!
    */
    property string ownerName: "(not set)"

    // read-only
    /*!
    */
    property real paintedWidth: imageComponent.paintedWidth
    /*!
    */
    property real paintedHeight: imageComponent.paintedHeight
    /*!
    */
    property bool isLoaded: false

    // internal
    /*!
    */
    property Image image: imageComponent

    /*!
    */
    signal loaded()

    clip: true

    Image {
        id: imageComponent

        width: parent.width
        height: parent.height
        x: 0
        y: 0

        source: (width > 0 && height > 0) ? photoComponent.source : ""

        // CRITICAL: when we moved to QT 5, we began to experience crashes when
        //           drawing photos after editing operations (see Launchpad bug
        //           #1065208). It turns out that, due to a bug in QT 5, in some
        //           cases images can be added to the list of objects to be drawn
        //           even though they're not yet loaded into memory. Of course,
        //           this causes a segfault. This property binding is here to
        //           prevent this segfault & crash from occurring.
        visible: isLoaded;

        sourceSize.width: width
        sourceSize.height: height

        // use cache: !isAnimate setting for flicker-free animations and reflows
        asynchronous: !isAnimate
        cache: !isAnimate
        smooth: !isAnimate
        fillMode: isCropped ? Image.PreserveAspectCrop : Image.PreserveAspectFit

        onStatusChanged: {
            if(status == Image.Ready) {
                isLoaded = true;
                loaded();
            } else {
                isLoaded = false;
            }
        }
    }
}

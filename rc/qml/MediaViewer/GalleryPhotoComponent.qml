/*
 * Copyright (C) 2011-2015 Canonical Ltd
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

import QtQuick 2.4

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
    property real paintedWidth: parent.width
    /*!
    */
    property real paintedHeight: parent.height
    /*!
    */
    property bool isLoaded: false

    // internal
    /*!
    */
    property Image image: null

    /*!
    */
    signal loaded()
}

/*
 * Copyright (C) 2011 Canonical Ltd
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
 */

import QtQuick 2.0
import Gallery 1.0
import "../../../js/Gallery.js" as Gallery

/*!
  Item to show a photo. It first loads very fast a preview of the photo. Afterwards a bigger version
  is loaded (max. size of the screen). This way a blank screen is avoided.
  If the isPreview property is true, only the preview is loaded.
  All image loading is done asynchronous.
*/
Item {
    id: root

    /// Link to the media to be shown
    property variant mediaSource
    /// Limit the view to show the preview version only, if set to true
    property bool isPreview: false
    /// As long as this load property is false, the loading of the images is not triggered
    property bool load: false
    /// True if the photo is visible, either as preview, or as full version
    property bool isLoaded: preview.status === Image.Ready || fullImage.status === Image.Ready

    Image {
        id: preview
        anchors.fill: parent
        asynchronous: true
        visible: fullImage.opacity < 1
        source: load && mediaSource ? "image://thumbnailer/" + mediaSource.path + "?at=" + Date.now() : ""
        fillMode: fullImage.fillMode
        sourceSize.width: 256

        Connections {
            target: mediaSource ? mediaSource : null
            onDataChanged: {
                // data changed but filename didn't, so we need to bypass the qml image
                // cache by tacking a timestamp to the filename so sees it as different.
                preview.source = "image://thumbnailer/" + mediaSource.path + "?at=" + Date.now()

                // reload full image
                var src = fullImage.source;
                fullImage.asynchronous = false;
                fullImage.source = "";
                fullImage.asynchronous = true;
                fullImage.source = src;
            }
        }
    }
    Image {
        id: fullImage
        anchors.fill: parent
        asynchronous: true
        cache: false
        fillMode: Image.PreserveAspectCrop
        source: (preview.status === Image.Ready && !isPreview) ?
                "image://thumbnailer/" + mediaSource.path : ""

        property int maxSize: Math.max(width, height)
        sourceSize.width: maxSize
        sourceSize.height: maxSize

        opacity: status === Image.Ready ? 1 : 0
        Behavior on opacity {
            NumberAnimation { duration: Gallery.SNAP_DURATION }
        }
    }
    Image {
        // Display a play icon if the thumbnail is from a video
        source: "../../../img/icon_play.png"
        anchors.centerIn: parent
        visible: isLoaded && mediaSource.type === MediaSource.Video
    }
}

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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
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
        source: load && mediaSource ? "image://thumbnailer/" + mediaSource.path + "?at=" + mediaSource.lastModified : ""
        fillMode: fullImage.fillMode
        sourceSize.width: 256
    }
    Image {
        id: fullImage
        anchors.fill: parent
        asynchronous: true
        cache: false
        fillMode: Image.PreserveAspectCrop
        source: (preview.status === Image.Ready && !isPreview) ?
                "image://thumbnailer/" + mediaSource.path + "?at=" + mediaSource.lastModified : ""

        property int maxSize: Math.max(width, height)
        sourceSize.width: maxSize
        sourceSize.height: maxSize

        opacity: status === Image.Ready ? 1 : 0
        Behavior on opacity {
            NumberAnimation { duration: Gallery.SNAP_DURATION }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        visible: preview.status == Image.Error

        Icon {
            anchors.centerIn: parent
            width: isPreview ? units.gu(4) : units.gu(6)
            height: width
            name: "stock_image"
            color: "white"
            opacity: 0.8
        }
    }

	Icon {
		// Display a play icon if the thumbnail is from a video
		anchors.centerIn: parent
		width: units.gu(5)
		height: units.gu(5)
		name: "media-playback-start"
		color: "white"
		visible: isLoaded && mediaSource.type === MediaSource.Video
	}
}

/*
 * Copyright (C) 2013 Canonical Ltd
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
 */

import QtQuick 2.0
import QtMultimedia 5.0
import Gallery 1.0

/*!
VideoViewerDelegate is an item to show a vide thumbnail, and play the video
if requrested
*/
Item {
    id: videoViewerDelegate

    /// The video item that to be shown by this component
    property MediaSource mediaSource
    /// Is true, once this component is fully usable
    property bool isLoaded: thumbnail.status === Image.Ready

    /// Stops the video playback if running
    function reset() {
        state = "stopped";
    }

    /// Starts playing the video
    function playVideo() {
        if (!loader_video.item)
            loader_video.sourceComponent = component_video;

        if (loader_video.item.source !== mediaSource.path)
            loader_video.item.source = mediaSource.path;
        loader_video.item.play();
    }

    /// Toggles between playing and pausing the video playback
    function togglePlayPause() {
        if (!loader_video.item)
            loader_video.sourceComponent = component_video;

        if (videoViewerDelegate.state === "playing") {
            loader_video.item.pause();
        } else {
            videoViewerDelegate.playVideo();
        }
    }

    Image {
        id: thumbnail

        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: mediaSource.galleryPreviewPath
        asynchronous: true
    }

    Image {
        // Display a play icon if the media is a video
        source: "../../img/icon_play.png"
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            videoViewerDelegate.togglePlayPause();
        }
    }

    Component {
        id: component_video
        Video {
            id: video
            onStopped: {
                videoViewerDelegate.state = "stopped";
            }
            onPaused: {
                videoViewerDelegate.state = "paused"
            }
            onPlaying: {
                videoViewerDelegate.state = "playing"
            }
        }
    }
    Loader {
        id: loader_video
        anchors.fill: parent
    }


    state: "stopped"
    states: [
        State {
            name: "playing"
            PropertyChanges { target: thumbnail; visible: false }
        },
        State {
            name: "paused"
            PropertyChanges { target: thumbnail; visible: false }
        },
        State {
            name: "stopped"
            PropertyChanges { target: thumbnail; visible: true }
        }
    ]
    onStateChanged: {
        if (state === "stopped") {
            if (loader_video.item)
                loader_video.item.stop()
            loader_video.sourceComponent = null
        }
    }
}

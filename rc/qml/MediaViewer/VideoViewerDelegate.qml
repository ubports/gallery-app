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
        video.stop();
    }

    /// Starts playing the video
    function playVideo() {
        if (video.source !== mediaSource.path)
            video.source = mediaSource.path;
        video.play();
    }

    /// Toggles between playing and pausing the video playback
    function togglePlayPause() {
        if (videoViewerDelegate.state === "playing") {
            video.pause();
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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            videoViewerDelegate.togglePlayPause();
        }
    }

    Video {
        id: video
        anchors.fill: parent
        visible: false
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

    state: "stopped"
    states: [
        State {
            name: "playing"
            PropertyChanges { target: thumbnail; visible: false }
            PropertyChanges { target: video; visible: true }
        },
        State {
            name: "paused"
            PropertyChanges { target: thumbnail; visible: false }
            PropertyChanges { target: video; visible: true }
        },
        State {
            name: "stopped"
            PropertyChanges { target: thumbnail; visible: true }
            PropertyChanges { target: video; visible: false }
        }
    ]
}

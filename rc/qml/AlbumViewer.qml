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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Gallery 1.0
import "../Capetown"
import "../js/Gallery.js" as Gallery
import "../js/GalleryUtility.js" as GalleryUtility
import "Components"
import "Utility"
import "Widgets"

/*!
*/
Page {
    id: albumViewer

    /// The album that is shown by this viewer
    property Album album

    // Read-only
    /*!
    */
    property alias pagesPerSpread: albumSpreadViewer.pagesPerSpread
    /*!
    */
    property bool animationRunning: albumSpreadViewer.isFlipping ||
                                    removeCrossfadeAnimation.running ||
                                    albumSpreadViewerForTransition.freeze ||
                                    photoViewer.animationRunning

    // Automatically hide the header when album viewer becomes active
    onActiveChanged: {
        if (active && albumViewer.header) {
            albumViewer.header.hide();
        }
    }

    // When the user clicks the back button or pages back to the cover.
    signal closeRequested(bool stayOpen, int viewingPage)

    Image {
        anchors.fill: parent
        source: "../img/background-paper.png"
    }

    state: "pageView"

    states: [
        State { name: "pageView"; },
        State { name: "gridView"; }
    ]

    transitions: [
        Transition { from: "pageView"; to: "gridView";
            ParallelAnimation {
                DissolveAnimation { fadeOutTarget: albumSpreadViewer; fadeInTarget: organicView; }
            }
        },
        Transition { from: "gridView"; to: "pageView";
            ParallelAnimation {
                DissolveAnimation { fadeOutTarget: organicView; fadeInTarget: albumSpreadViewer; }
            }
        }
    ]

    onStateChanged: {
        if (state == "pageView")
            organicView.selection.leaveSelectionMode();
    }

    function crossfadeRemove() {
        removeCrossfadeAnimation.restart();
    }

    function fadeOutAndFlipRemove(flipToPage) {
        fadeOutAnimation.flipToPage = flipToPage;
        fadeOutAnimation.restart();
    }

    /// Closes the view as stores the page number the page number currently viewed
    function __close() {
        closeRequested(album.containedCount > 0, albumSpreadViewer.viewingPage)
    }

    FadeOutAnimation {
        id: fadeOutAnimation

        property int flipToPage: 0

        target: isPortrait ? albumSpreadViewerForTransition.rightPageComponent :
                             albumSpreadViewerForTransition.leftPageComponent
        duration: 200
        easingType: Easing.Linear

        onRunningChanged: {
            if (running)
                return;

            flipTimer.restart()

            albumSpreadViewerForTransition.flipTo(flipToPage);
            albumSpreadViewer.flipTo(flipToPage);
        }
    }

    Timer {
        id: flipTimer

        interval: albumSpreadViewer.duration

        onTriggered: albumSpreadViewerForTransition.freeze = false
    }

    DissolveAnimation {
        id: removeCrossfadeAnimation

        duration: 200
        fadeOutTarget: albumSpreadViewerForTransition
        fadeInTarget: albumSpreadViewer
        easingType: Easing.Linear

        onRunningChanged: {
            if (running)
                return;

            albumSpreadViewerForTransition.freeze = false;
        }
    }

    function resetView(album) {
        albumViewer.album = album;

        state = ""; // Prevents the animation on gridView -> pageView from happening.
        state = "pageView";

        albumSpreadViewer.visible = true;
        chrome.resetVisibility(false);
        organicView.visible = false;

        albumSpreadViewer.viewingPage = album.currentPage;
    }

    // Used for the cross-fade transition.
    AlbumSpreadViewer {
        id: albumSpreadViewerForTransition

        anchors.fill: parent
        album: albumViewer.album
        z: 100
        visible: freeze
        load: parent.visible && freeze && parent.state == "pageView"

        Connections {
            target: albumSpreadViewer

            onViewingPageChanged: {
                if (!albumSpreadViewerForTransition.freeze)
                    albumSpreadViewerForTransition.flipTo(albumSpreadViewer.viewingPage);
            }
        }

        onFreezeChanged: {
            if (!freeze)
                flipTo(albumSpreadViewer.viewingPage);
        }
    }

    AlbumSpreadViewer {
        id: albumSpreadViewer

        anchors.fill: parent

        album: albumViewer.album
        load: parent.visible && parent.state == "pageView"

        // Keyboard focus while visible and viewer is not visible
        focus: visible //&& !photoViewer.isPoppedUp

        showCover: !albumSpreadViewerForTransition.freeze

        Keys.onPressed: {
            if (event.key !== Qt.Key_Left && event.key !== Qt.Key_Right)
                return;

            var direction = (event.key === Qt.Key_Left ? -1 : 1);
            var destination = albumSpreadViewer.viewingPage +
                    direction * albumSpreadViewer.pagesPerSpread;

            if (!albumSpreadViewer.isFlipping &&
                    albumSpreadViewer.isPopulatedContentPage(destination)) {
                chrome.hide(true);

                albumSpreadViewerForTransition.flipTo(destination);

                event.accepted = true;
            }
        }

        SwipeArea {
            property real commitTurnFraction: 0.05

            // Should be treated as internal to this SwipeArea. Used for
            // tracking the last swipe direction; if the user moves a page
            // past the 'commit' point, then back, we'll track that here,
            // treat it as a request to release the page and go back to idling.
            //
            // Per the convention used elsewhere, true for right, false for left.
            property bool lastSwipeLeftToRight: true
            property int prevSwipingX: -1

            // Normal press/click.
            function pressed(x, y) {
                var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
                if (!hit)
                    return;

                // Handle add button.
                if (hit.objectName === "addButton")
                    loader_mediaSelector.show();

                if (!hit.mediaSource)
                    return;

                if (organicView.selection.inSelectionMode) {
                    organicView.selection.toggleSelection(hit.mediaSource);
                } else {
                    photoViewer.forGridView = false;
                    photoViewer.fadeOpen(hit.mediaSource);
                }
            }

            // Long press/right click.
            function alternativePressed(x, y) {
                var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
                if (!hit || !hit.mediaSource)
                    return;

                // FIXME show a popover menu for that photo
            }

            anchors.fill: parent

            enabled: !parent.isRunning

            onTapped: {
                if (rightButton)
                    alternativePressed(x, y);
                else
                    pressed(x, y);
            }
            onLongPressed: alternativePressed(x, y)

            onStartSwipe: {
                var direction = (leftToRight ? -1 : 1);
                albumSpreadViewer.destinationPage =
                        albumSpreadViewer.viewingPage +
                        direction * albumSpreadViewer.pagesPerSpread;

                prevSwipingX = mouseX;
            }

            onSwiping: {
                lastSwipeLeftToRight = (mouseX > prevSwipingX);

                var availableDistance = (leftToRight) ? (width - start) : start;
                // TODO: the 0.999 here is kind of a hack.  The AlbumPageFlipper
                // can't tell the difference between its flipFraction being set to 1
                // from the drag vs. its own animation.  So I don't let the drag set it
                // quite all the way to 1.  I should somehow fix this shortcoming in
                // the AlbumPageFlipper, but this is fine for now.
                var flipFraction =
                        Math.max(0, Math.min(0.999, distance / availableDistance));
                if (!albumSpreadViewer.isPopulatedContentPage(albumSpreadViewer.destinationPage)) {
                    var maxFraction = 0.425
                    if (isPortrait) {
                        if (albumSpreadViewer.destinationPage === 0)
                            maxFraction = 0.15 // for the front
                        else
                            maxFraction = 0.75 // for the back
                    }

                    flipFraction = Math.min(maxFraction, flipFraction)
                }
                albumSpreadViewer.flipFraction = flipFraction;
                prevSwipingX = mouseX;
            }

            onSwiped: {
                // Can turn toward the cover, but never close the album in the viewer
                var minValidPage = album.firstValidCurrentPage
                var maxValidPage = album.lastValidCurrentPage
                if (isPortrait) {
                    minValidPage += 1
                    maxValidPage -= 1
                }
                if (albumSpreadViewer.flipFraction >= commitTurnFraction &&
                        leftToRight === lastSwipeLeftToRight &&
                        albumSpreadViewer.destinationPage > minValidPage &&
                        albumSpreadViewer.destinationPage < maxValidPage) {
                    albumSpreadViewer.flip();
                }
                else
                    albumSpreadViewer.release();
            }
        }
    }

    OrganicAlbumView {
        id: organicView

        anchors.fill: parent

        visible: false

        // avoid overriding the toolbar and header
        active: false

        album: albumViewer.album

        onMediaSourcePressed: {
            var rect = GalleryUtility.translateRect(thumbnailRect, organicView, photoViewer);
            photoViewer.forGridView = true;
            photoViewer.animateOpen(mediaSource, rect);
        }

        Image {
            id: plusButton

            anchors.centerIn: parent

            visible: album !== null && album.containedCount == 0

            source: "Components/AlbumInternals/img/album-add.png"

            MouseArea {
                anchors.fill: parent
                onClicked: loader_mediaSelector.show()
            }
        }
    }

    PopupPhotoViewer {
        id: photoViewer

        // true if the grid view component is using the photo viewer, false if the
        // album spread viewer is using it ... this should be set prior to
        // opening the viewer
        property bool forGridView
        model: MediaCollectionModel {
            forCollection: albumViewer.album
        }

        album: albumViewer.album

        anchors.fill: parent
        visible: false

        onOpening: {
            visible = true;
            albumViewer.active = false;
        }

        onOpened: {
            albumSpreadViewer.visible = false;
            organicView.visible = false;
        }

        onCloseRequested: {
            if (forGridView) {
                // TODO: position organicView.
            } else {
                var page = albumViewer.album.getPageForMediaSource(photo);
                if (page >= 0) {
                    albumViewer.album.currentPage = albumSpreadViewer.getLeftHandPageNumber(page);
                    albumSpreadViewer.viewingPage = isPortrait? page : albumViewer.album.currentPage;
                }
            }

            albumSpreadViewer.visible = (albumViewer.state == "pageView");
            organicView.visible = (albumViewer.state == "gridView");

            if (forGridView) {
                var rect = null; // TODO: get rect from organicView.
                if (rect)
                    animateClosed(rect);
                else
                    close();
            } else {
                fadeClosed();
            }
        }

        onClosed: {
            visible = false;
            albumViewer.active = true;
        }
    }

    Component {
        id: component_mediaSelector
        MediaSelector {
            id: mediaSelector

            onAddClicked: {
                var album  = albumViewer.album;
                var firstPhoto = album.addSelectedMediaSources(selection.model);
                if (firstPhoto && albumViewer.state == "pageView") {
                    var firstChangedPage = album.getPageForMediaSource(firstPhoto);
                    var firstChangedSpread = albumSpreadViewer.getLeftHandPageNumber(firstChangedPage);
                    albumSpreadViewer.flipTo(firstChangedSpread);
                }
            }

            onHidden: {
                loader_mediaSelector.sourceComponent = undefined;
                albumViewer.active = true;
            }
        }
    }
    Loader {
        id: loader_mediaSelector
        objectName: "albumMediaSelectorLoader"
        anchors.fill: parent
        function show() {
            sourceComponent = component_mediaSelector;
            item.show();
            albumViewer.active = false;
        }
    }

    DeleteOrDeleteWithContentsDialog {
        id: albumTrashDialog
        visible: false
        onDeleteClicked: {
            // FIXME do not show the close animation
            __close()
        }
        onDeleteWithContentsClicked: {
            // FIXME do not show the close animation
            __close()
        }
    }

    /// Contains the actions for the toolbar in the album view
    tools: ToolbarActions {
        Action {
            text: "Add"
            iconSource: Qt.resolvedUrl("../img/add.png")
            onTriggered: {
                loader_mediaSelector.show()
            }
        }
        Action {
            text: "Delete"
            iconSource: Qt.resolvedUrl("../img/delete.png")
            onTriggered: {
                albumTrashDialog.album = album
                albumTrashDialog.caller = caller
                albumTrashDialog.show()
            }
            enabled: false // FIXME enable once the close animation is not shown anymore
        }
        Action {
            text: "Share"
            iconSource: Qt.resolvedUrl("../img/share.png")
            enabled: false
        }
        back: Action {
            text: "Back"
            iconSource: Qt.resolvedUrl("../img/back.png")
            onTriggered: {
                __close()
            }
        }
    }
}

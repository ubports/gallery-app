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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Gallery 1.0
import "../../js/Gallery.js" as Gallery
import "../../js/GalleryUtility.js" as GalleryUtility
import "../Components"
import "../Utility"

/*!
*/
Page {
    id: albumViewer

    /// The album that is shown by this viewer
    property Album album

    /// This property exists only for the benefit of autopilot tests
    /// which cannot access the album property directy as it's not primitive
    property int albumId: (album) ? album.id : -1

    // Read-only
    /*!
    */
    property alias pagesPerSpread: albumSpreadViewer.pagesPerSpread
    /*!
    */
    property bool animationRunning: albumSpreadViewer.isFlipping ||
                                    photoViewerLoader.animationRunning

    property string mediaCurrentlyInView
    StateSaver.properties: "mediaCurrentlyInView"

    /// Indicates if this view is open
    property bool isOpen: false
    /// Origin (rectangle) where this view is animated from when calling open()
    /// And where it is animated to when this view is closed
    property variant origin
    /// The preview item that was used to open the view, or null, if it was opend another way
    /// This item will be hidden when this view is opened. And shown again after closing this view
    property Item previewItem: null

    function reopenPicture() {
        if (album && albumViewer.mediaCurrentlyInView !== "") {
            for (var i in album.allMediaSources) {
                var source = album.allMediaSources[i];

                if (source.path == albumViewer.mediaCurrentlyInView) {
                    photoViewerLoader.fadeOpen(source);
                    return;
                }
            }
        }
    }

    onVisibleChanged: {
        if (visible) reopenPicture();
    }

    onCloseRequested: {
        if (origin) {
            if (album)
                album.closed = !stayOpen || (viewingPage == 0);
            if (previewItem)
                previewItem.visible = true
        }
        visible = false
        if (!APP.desktopMode)
            setFullScreenAppMode(false);
        overview.popPage();
        isOpen = false
    }
    onQuickCloseRequested: {
        visible = false
        if (!APP.desktopMode)
            setFullScreenAppMode(false);
        overview.popPage();
        isOpen = false
        if (previewItem)
            previewItem.visible = true;
    }

    // When the user clicks the back button or pages back to the cover.
    signal closeRequested(bool stayOpen, int viewingPage)
    // When the user clicks the back button or pages back to the cover.
    signal quickCloseRequested()

    title: i18n.tr("Album")

    function closeAlbum() {
        if (photoViewerLoader.item && photoViewerLoader.item.isPoppedUp) {
            photoViewerLoader.item.closePopupPhotoViewer();
        }
        __close();
    }

    /// Closes the view as stores the page number the page number currently viewed
    function __close() {
        closeRequested(album.containedCount > 0, albumSpreadViewer.viewingPage)
    }

    /// Closes the view without animation or storing anything
    function __quickClose() {
        quickCloseRequested()
    }

    function resetView(album) {
        albumViewer.album = album;

        albumSpreadViewer.visible = true;
        chrome.resetVisibility(false);
        organicView.visible = false;

        albumSpreadViewer.viewingPage = album.currentPage;
    }

    function showMediaSelector() {
        overview.pushPage(component_mediaSelector);
    }

    AlbumSpreadViewer {
        id: albumSpreadViewer
        objectName: "spreadViewer"

        anchors.fill: parent

        album: albumViewer.album
        load: parent.visible

        // Keyboard focus while visible and viewer is not visible
        focus: visible

        showCover: true

        Keys.onPressed: {
            if (event.key !== Qt.Key_Left && event.key !== Qt.Key_Right)
                return;

            var direction = (event.key === Qt.Key_Left ? -1 : 1);
            var destination = albumSpreadViewer.viewingPage +
                    direction * albumSpreadViewer.pagesPerSpread;

            if (!albumSpreadViewer.isFlipping &&
                    albumSpreadViewer.isPopulatedContentPage(destination)) {
                chrome.hide(true);

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
            property bool canceled: false

            // Normal press/click.
            function pressed(x, y) {
                var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
                if (!hit)
                    return;

                // Handle add button.
                if (hit.objectName === "addButton")
                    showMediaSelector();
                else if (!hit.mediaSource) {
                    albumViewer.header.visible = !albumViewer.header.visible;
                    if (!APP.desktopMode)
                        setFullScreenAppMode(!albumViewer.header.visible);
                    return;
                }

                albumViewer.mediaCurrentlyInView = hit.mediaSource.path;
                photoViewerLoader.fadeOpen(hit.mediaSource);
                photoViewerLoader.item.header.visible = false;
                if (!APP.desktopMode)
                    setFullScreenAppMode(true);
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
                var destination = albumSpreadViewer.viewingPage +
                                  direction * albumSpreadViewer.pagesPerSpread;

                if ((destination > album.firstValidCurrentPage ||
                     pagesPerSpread == 2 && destination == album.firstValidCurrentPage) &&
                    destination <= album.lastPopulatedContentPage) {
                    albumSpreadViewer.destinationPage = destination;
                    prevSwipingX = mouseX;
                } else {
                    canceled = true;
                }
            }

            onSwiping: {
                if (canceled) return;
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
                    var maxFraction = (isPortrait) ? 0.75 : 0.425
                    flipFraction = Math.min(maxFraction, flipFraction)
                }
                albumSpreadViewer.flipFraction = flipFraction;
                prevSwipingX = mouseX;
            }

            onSwiped: {
                if (canceled) {
                    canceled = false;
                    return;
                }
                var minValidPage = album.firstValidCurrentPage
                var maxValidPage = album.lastValidCurrentPage
                if (isPortrait) {
                    minValidPage += 1
                    maxValidPage -= 1
                }
                if (albumSpreadViewer.flipFraction >= commitTurnFraction &&
                        leftToRight === lastSwipeLeftToRight) {
                    if (albumSpreadViewer.destinationPage >= minValidPage &&
                               albumSpreadViewer.destinationPage <= maxValidPage) {
                        albumSpreadViewer.flip();
                    } else albumSpreadViewer.release();
                } else albumSpreadViewer.release();
            }
        }
    }

    Loader {
        id: photoViewerLoader
        anchors.fill: parent

        /// Is true while the view is popping up or closing
        property bool animationRunning: item ? item.animationRunning : false


        /// Opens the view
        function fadeOpen(media) {
            if (status === Loader.Null)
                setSource(Qt.resolvedUrl("../MediaViewer/PopupPhotoViewer.qml"),
                          {model: albumCollection, album: albumViewer.album})
            photoViewerLoader.item.fadeOpen(media)
        }

        MediaCollectionModel {
            id: albumCollection
            forCollection: albumViewer.album
        }

        Connections{
            target: photoViewerLoader.item
            onOpening: {
                photoViewerLoader.visible = true;
            }

            onOpened: {
                photoViewerLoader.item.title = albumViewer.title;
                overview.pushPage(target);
            }
            onCloseRequested: {
                albumViewer.mediaCurrentlyInView = "";
                var page = albumViewer.album.getPageForMediaSource(photoViewerLoader.item.photo);
                if (page >= 0) {
                    albumViewer.album.currentPage = albumSpreadViewer.getLeftHandPageNumber(page);
                    albumSpreadViewer.viewingPage = isPortrait? page : albumViewer.album.currentPage;
                }

                photoViewerLoader.item.fadeClosed();
                albumViewer.header.visible = true;
            }
            onClosed: {
                overview.popPage();
            }

        }
    }

    Component {
        id: component_mediaSelector
        MediaSelector {
            id: mediaSelector

            onAddClicked: {
                var album  = albumViewer.album;
                var firstPhoto = album.addSelectedMediaSources(selection.model.selectedMedias);
                if (firstPhoto && albumViewer.state == "pageView") {
                    var firstChangedPage = album.getPageForMediaSource(firstPhoto);
                    var firstChangedSpread = albumSpreadViewer.getLeftHandPageNumber(firstChangedPage);
                    albumSpreadViewer.flipTo(firstChangedSpread);
                }
            }

            onHidden: {
                overview.popPage();
            }
        }
    }

    DeleteOrDeleteWithContentsDialog {
        id: albumTrashDialog
        visible: false
        onDeleteClicked: {
            __quickClose();
        }
        onDeleteWithContentsClicked: {
            __quickClose();
        }
    }

    header: PageHeader {
        objectName: "albumViewerHeader"
        title: albumViewer.title
        /// Contains the actions for the toolbar in the album view
        trailingActionBar.actions: [
            Action {
                objectName: "addButton"
                text: i18n.tr("Add to album") // text in HUD
                iconName: "add"
                onTriggered: showMediaSelector();
            },
            Action {
                objectName: "deleteButton"
                text: i18n.tr("Delete")
                iconName: "delete"
                onTriggered: {
                    albumTrashDialog.album = album;
                    albumTrashDialog.show();
                }
            }
        ]

        leadingActionBar.actions: Action {
            objectName: "backButton"
            iconName: "back"
            onTriggered: __close();
        }
    }
}

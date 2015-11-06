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
 * Jim Nelson <jim@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4
import Gallery 1.0
import "../../js/GalleryUtility.js" as GalleryUtility
import "AlbumInternals"

// The AlbumSpreadViewer is a specialty controller for displaying and viewing
// albums one spread (two pages side by side like an open book) at a time, and
// also one page at a time in the "magical" portrait layout where every page is
// on the right hand side of the album.  Also encapsulates the logic of turning
// the pages from a swipe or via a page indicator button (see AlbumPageFlipper
// for details).
Item {
    id: albumSpreadViewer

    /*!
    */
    signal pageReleased()
    /*!
    */
    signal pageFlipped()

    // public
    /*!
    */
    property Album album
    /*!
    */
    property alias destinationPage: flipper.destinationPage
    /*!
    */
    property alias duration: flipper.duration
    /*!
    */
    property alias flipFraction: flipper.flipFraction
    /*!
    */
    property bool load: false
    /*!
    */
    property bool freeze: false   // See AlbumPageComponent
    /*!
    */
    property bool showCover: true // See AlbumPageComponent

    // readonly
    /*!
    */
    property alias isFlipping: flipper.isFlipping
    /*!
    */
    property alias isRunning: flipper.isRunning
    /*!
    */
    property bool isPortrait: application.isPortrait
    // Same as album.currentPage, unless we're in portrait mode in which case it
    // can be either the left OR right page.
    property int viewingPage: -1
    /*!
    */
    property int pagesPerSpread: (isPortrait ? 1 : 2)
    /*!
    */
    property alias leftPageComponent: left
    /*!
    */
    property alias rightPageComponent: right

    /*!
      */
    function flip() {
        flipper.flipToDestination();
    }

    /*!
    */
    function release() {
        flipper.flipToOrigin();
    }

    /*!
    */
    function flipTo(page) {
        destinationPage = page;
        flipFraction = 0;
        flip();
    }

    // Returns whether the page is a content page that has any photos on it.
    function isPopulatedContentPage(page) {
        return (album && page >= album.firstContentPage
                && page <= album.lastPopulatedContentPage);
    }

    // Converts a page number into the appropriate page number to place on the
    // left-hand side of the component
    function getLeftHandPageNumber(pageNumber) {
        if (pageNumber <= album.firstValidCurrentPage)
            return album.firstValidCurrentPage;

        if (pageNumber >= album.lastValidCurrentPage)
            return album.lastValidCurrentPage;

        return GalleryUtility.isOdd(pageNumber) ? pageNumber : pageNumber - 1;
    }

    // public
    /*!
    */
    function hitTestFrame(x, y, relativeTo) {
        // current visible photos are on the back of the left page and the front
        // of the right page
        var hit = hitTestPage(left.back, x, y, relativeTo);
        if (hit)
            return hit;

        return hitTestPage(right.front, x, y, relativeTo);
    }

    // internal
    /*!
    */
    function hitTestPage(page, x, y, relativeTo) {
        if (!page.mediaFrames)
            return undefined;

        var ctr;
        for (ctr = 0; ctr < page.mediaFrames.length; ctr++) {
            var rect = GalleryUtility.getRectRelativeTo(page.mediaFrames[ctr], relativeTo);
            if (GalleryUtility.doesPointIntersectRect(x, y, rect))
                return page.mediaFrames[ctr];
        }

        return undefined;
    }

    // public
    /*!
    */
    function getRectOfMediaSource(media) {
        // current visible photos are on the back of the left page and the front of
        // the right page
        var rect = searchPageForMedia(left.back, media);
        if (rect)
            return rect;

        return searchPageForMedia(right.front, media);
    }

    // private
    /*!
    */
    function searchPageForMedia(page, media) {
        if (!page.mediaFrames)
            return undefined;

        var ctr;
        for (ctr = 0; ctr < page.mediaFrames.length; ctr++) {
            if (page.mediaFrames[ctr].mediaSource && page.mediaFrames[ctr].mediaSource.equals(media))
                return page.mediaFrames[ctr];
        }

        return undefined;
    }

    onAlbumChanged: viewingPage = (album ? album.currentPage : -1)
    onIsPortraitChanged: {
        // Make sure we're on a valid currentPage when switching back to regular
        // mode from portrait.
        if (!isPortrait && GalleryUtility.isEven(viewingPage))
            --viewingPage;
    }

    Item {
        id: pageArea

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: (isPortrait ? parent.left : parent.horizontalCenter)
        anchors.right: parent.right

        AlbumPageComponent {
            id: left

            anchors.fill: parent

            visible: freeze || (backPage >= 0 && !isPortrait)
            load: albumSpreadViewer.load

            album: albumSpreadViewer.album

            backPage: leftPageForCurrent(flipper.isFlipInProgress
                                         ? flipper.firstPage
                                         : viewingPage)

            flipFraction: 1
            freeze: albumSpreadViewer.freeze
            showCover: albumSpreadViewer.showCover
        }

        AlbumPageComponent {
            id: right

            anchors.fill: parent

            visible: freeze || (Boolean(album) && frontPage < album.totalPageCount)
            load: albumSpreadViewer.load

            album: albumSpreadViewer.album

            frontPage: {
                var page = (flipper.isFlipInProgress ? flipper.lastPage : viewingPage);
                return (isPortrait ? page : rightPageForCurrent(page));
            }

            flipFraction: 0
            freeze: albumSpreadViewer.freeze
            showCover: albumSpreadViewer.showCover
        }

        AlbumPageFlipper {
            id: flipper

            // Read only.
            // Used instead of isFlipping to enforce the proper order of operations;
            // isFlipping will be set to false before the page number is updated in
            // onFlipFinished, resulting in an extraneous page reload.
            property bool isFlipInProgress: false

            anchors.fill: parent

            visible: isFlipInProgress

            album: albumSpreadViewer.album
            currentPage: viewingPage
            isPortrait: albumSpreadViewer.isPortrait
            pagesPerSpread: albumSpreadViewer.pagesPerSpread

            onIsFlippingChanged: {
                if (isFlipping)
                    isFlipInProgress = true;
            }

            onFlipFinished: {
                if (!album)
                    return;

                if (toDestination) {
                    if (destinationPage == album.firstValidCurrentPage) {
                        album.closed = true;
                    } else {
                        album.currentPage = getLeftHandPageNumber(destinationPage);
                        album.closed = false;
                    }
                    viewingPage = destinationPage;
                    isFlipInProgress = false; // must be set AFTER viewingPage!
                    pageFlipped();
                } else {
                    pageReleased();
                }
            }
        }
    }
}

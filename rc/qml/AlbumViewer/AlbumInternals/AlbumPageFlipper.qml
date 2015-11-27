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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4
import Gallery 1.0
import "../../../js/Gallery.js" as Gallery

// A group of animated pages flippable from the AlbumSpreadViewer.  Because of
// how there are multiple pages to be flipped at once, keyed off a single
// incremental value, this involves a lot of non-obvious math internally.  The
// interface is simple, though: set the album and destinationPage, then simply
// move flipFraction from 0 -> 1 to incrementally flip the page(s) or call
// turnToDestination() or ...Origin() to animate it automatically.
Item {
    id: albumPageFlipper

    /*!
    */
    signal flipFinished(bool toDestination)

    /*!
    */
    property Album album
    /*!
    */
    property int currentPage
    /*!
    */
    property int destinationPage
    /*!
    */
    property int duration: Gallery.FAST_DURATION
    /*!
    */
    property real flipFraction
    /*!
    */
    property bool isPortrait
    /*!
    */
    property int pagesPerSpread

    // readonly
    /*!
    */
    property bool isFlipping: (flipFraction != 0 && flipFraction != 1)
    /*!
    */
    property bool isForward: (destinationPage > currentPage)
    /*!
    */
    property int firstPage: (isForward ? currentPage : destinationPage)
    /*!
    */
    property int lastPage: (isForward ? destinationPage : currentPage)
    /*!
    */
    property alias isRunning: animator.running

    // internal
    /*!
    */
    property int maxPages: 5
    /*!
    */
    property int numPages: Math.min(maxPages,
                                    Math.abs(destinationPage - currentPage) /
                                    pagesPerSpread)
    /*!
    */
    property real pageFlipFraction: (isForward ? flipFraction : 1 - flipFraction)
    /*!
    */
    property real gapBetweenPages: (isPortrait ? 0.12 : 0.1) // Eyeballed.
    /*!
    */
    property real flipSlope: 1 / (1 - gapBetweenPages * (numPages - 1))

    /*!
    */
    function flipToDestination() {
        animator.to = 1;
        animator.duration = (1 - flipFraction) * duration;
        animator.restart();
    }

    /*!
    */
    function flipToOrigin() {
        animator.to = 0;
        animator.duration = flipFraction * duration;
        animator.restart();
    }

    AlbumPageFlipperPage {
        flipperPage: 0
    }

    AlbumPageFlipperPage {
        flipperPage: 1
    }

    AlbumPageFlipperPage {
        flipperPage: 2
    }

    AlbumPageFlipperPage {
        flipperPage: 3
    }

    AlbumPageFlipperPage {
        flipperPage: 4
    }

    NumberAnimation {
        id: animator

        target: albumPageFlipper
        property: "flipFraction"
        easing.type: Easing.OutQuad

        onRunningChanged: {
            if (!running)
                flipFinished(flipFraction == 1);
        }
    }
}

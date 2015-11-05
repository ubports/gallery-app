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
import ".."

// A page included inside the AlbumPageFlipper; see notes there.  Tightly
// coupled.
AlbumPageComponent {
    id: albumPageFlipperPage

    /*!
    */
    property int flipperPage

    // internal
    /*!
    */
    property int page // For some reason QML chokes unless this is broken up onto
    page: {           // two lines like so.
        // Last page's number is one spread back from lastPage.
        if (flipperPage == numPages - 1)
            return lastPage - pagesPerSpread;

        // Second to last page's number is two spreads back.
        if (flipperPage == numPages - 2)
            return lastPage - 2 * pagesPerSpread;

        // Otherwise, we do sequential spreads forward from firstPage.
        return firstPage + pagesPerSpread * flipperPage;
    }

    property real flipFractionIntercept:
        -flipSlope * flipperPage * gapBetweenPages

    // These are from AlbumPageFlipper.
    /*!
    */
    property int numPages: parent.numPages
    /*!
    */
    property int firstPage: parent.firstPage
    /*!
    */
    property int lastPage: parent.lastPage
    /*!
    */
    property real gapBetweenPages: parent.gapBetweenPages
    /*!
    */
    property real pageFlipFraction: parent.pageFlipFraction
    /*!
    */
    property real flipSlope: parent.flipSlope
    /*!
    */
    property bool isPortrait: parent.isPortrait
    /*!
    */
    property int pagesPerSpread: parent.pagesPerSpread

    anchors.fill: parent
    z: flipFraction < 0.5 ? numPages - flipperPage : flipperPage

    visible: numPages > flipperPage
    load: visible

    album: parent.album

    frontPage: (isPortrait ? page : rightPageForCurrent(page))
    // backPage is the next spread or blank for portrait mode.
    backPage: (isPortrait ? -1 : leftPageForCurrent(page + pagesPerSpread))

    frontIsPreview: flipperPage > 0
    backIsPreview: numPages > flipperPage + 1

    // The 0.55 factor causes the animation to end juuuuust past vertical in
    // portrait mode.  This looks better to my eyes than directly vertical (0.5).
    flipFraction: (isPortrait ? 0.55 : 1) * Math.min(1, Math.max(0,
                                                                 flipSlope * pageFlipFraction + flipFractionIntercept))
}

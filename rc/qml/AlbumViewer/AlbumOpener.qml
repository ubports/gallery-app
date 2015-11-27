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

// A component to show an album thumbnail in closed or open state, and
// transition between the two.
Item {
    id: albumOpener

    // public
    /*!
    */
    property Album album
    /*!
    */
    property bool isPreview: false
    /*!
    */
    property bool load: false
    /*!
    */
    property bool contentHasPreviewFrame: false
    /*!
    */
    property int duration: 1000
    /*!
    */
    property real openFraction: 0
    // Whether to draw the cover (defaults to true)
    // This is used to prevent the cover from being drawn during certain transitions.
    property bool showCover: true
    /// Show the album closed, even if marked open in the DB
    property bool showClosed: false
    /*!
    */
    property int insideLeftPage: defaultInsideLeftPage
    /*!
    */
    property int insideRightPage: defaultInsideRightPage

    /*!
    */
    property real topMargin: previewTopMargin
    /*!
    */
    property real bottomMargin: previewBottomMargin
    /*!
    */
    property real gutterMargin: previewGutterMargin
    /*!
    */
    property real outerMargin: previewOuterMargin
    /*!
    */
    property real insideMargin: previewInsideMargin

    // readonly
    /*!
    */
    property bool isFlipping: (openFraction != 0 && openFraction != 1)
    /*!
    */
    property alias frameToContentWidth: rightPage.frameToContentWidth
    /*!
    */
    property alias frameToContentHeight: rightPage.frameToContentHeight

    /*!
    */
    property int defaultInsideLeftPage: leftPage.leftPageForCurrent(currentOrFirstContentPage)
    /*!
    */
    property int defaultInsideRightPage: rightPage.rightPageForCurrent(currentOrFirstContentPage)

    /*!
    */
    property alias pageTopMargin: rightPage.pageTopMargin
    /*!
    */
    property alias pageBottomMargin: rightPage.pageBottomMargin
    /*!
    */
    property alias pageGutterMargin: rightPage.pageGutterMargin
    /*!
    */
    property alias pageOuterMargin: rightPage.pageOuterMargin
    /*!
    */
    property alias pageInsideMargin: rightPage.pageInsideMargin

    /*!
    */
    property alias previewTopMargin: rightPage.previewTopMargin
    /*!
    */
    property alias previewBottomMargin: rightPage.previewBottomMargin
    /*!
    */
    property alias previewGutterMargin: rightPage.previewGutterMargin
    /*!
    */
    property alias previewOuterMargin: rightPage.previewOuterMargin
    /*!
    */
    property alias previewInsideMargin: rightPage.previewInsideMargin

    // internal
    /*!
    */
    property int currentOrFirstContentPage: (!album
                                             ? -1
                                             : (album.currentPage == album.firstValidCurrentPage
                                                ? album.firstContentPage
                                                : album.currentPage))

    /*!
    */
    function open() {
        animator.to = 1;
        animator.duration = (1 - openFraction) * duration;
        animator.restart();
    }

    /*!
    */
    function close() {
        animator.to = 0;
        animator.duration = openFraction * duration;
        animator.restart();
    }

    onAlbumChanged: openFraction = (!album || album.closed || showClosed ? 0 : 1)

    Connections {
        target: album
        ignoreUnknownSignals: true
        onClosedChanged: openFraction = (album.closed || showClosed ? 0 : 1)
    }

    Item {
        id: shifter

        // Shift it over as it opens so the visuals stay centered.
        x: (width * openFraction)
        y: -5 // Top of page frame
        width: parent.width
        height: parent.height

        AlbumPageComponent {
            id: rightPage

            anchors.fill: parent
            visible: (openFraction > 0 && openFraction < 1)
            load: albumOpener.load

            album: albumOpener.album
            frontPage: insideRightPage
            backPage: (album ? leftPageForCurrent(album.lastValidCurrentPage) : -1)

            isPreview: albumOpener.isPreview
            contentHasPreviewFrame: albumOpener.contentHasPreviewFrame

            flipFraction: (openFraction > 0.5 && openFraction <= 1 ? openFraction * -2 + 3 : 0)

            topMargin: albumOpener.topMargin
            bottomMargin: albumOpener.bottomMargin
            gutterMargin: albumOpener.gutterMargin
            outerMargin: albumOpener.outerMargin
            insideMargin: albumOpener.insideMargin
            showCover: albumOpener.showCover
        }

        AlbumPageComponent {
            id: leftPage

            anchors.fill: parent
            load: albumOpener.load

            album: albumOpener.album
            frontPage: (album ? rightPageForCurrent(album.firstValidCurrentPage) : -1)
            backPage: insideLeftPage

            isPreview: albumOpener.isPreview
            contentHasPreviewFrame: albumOpener.contentHasPreviewFrame

            flipFraction: (openFraction >= 0 && openFraction < 0.5 ? openFraction * 2 : 1)

            topMargin: albumOpener.topMargin
            bottomMargin: albumOpener.bottomMargin
            gutterMargin: albumOpener.gutterMargin
            outerMargin: albumOpener.outerMargin
            insideMargin: albumOpener.insideMargin
            showCover: albumOpener.showCover
        }
    }

    NumberAnimation {
        id: animator

        target: albumOpener
        property: "openFraction"
        easing.type: Easing.OutQuad

        onRunningChanged: {
            if (running)
                return;

            if (openFraction == 0) {
                album.closed = true;
            } else if (openFraction == 1) {
                album.closed = false;
                if (album.currentPage == album.firstValidCurrentPage)
                    album.currentPage = album.firstContentPage;
            }
        }
    }
}

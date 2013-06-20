/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import "../Components"

// AlbumViewerTransitionPortraitPage is just like AlbumOpener, except it only 
// shows one page at a time.  This is intended for use in AlbumViewerTransition
// as a replacement for AlbumOpener when used in portrait mode and closing
// an even-numbered page.
Item {
    id: albumViewerTransitionPortraitPage

    // public
    property Album album
    /*!
    */
    property bool isPreview: false
    /*!
    */
    property bool contentHasPreviewFrame: false
    /*!
    */
    property int viewingPage

    /*!
    */
    property int insideLeftPage: defaultInsideLeftPage
    /*!
    */
    property int insideRightPage: defaultInsideRightPage

    // Here, openFraction refers to how "flipped" the page is, rather than
    // how open the album is.
    property real openFraction: 0

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
    property alias frameToContentWidth: page.frameToContentWidth
    /*!
    */
    property alias frameToContentHeight: page.frameToContentHeight

    /*!
    */
    property alias load: page.load

    /*!
    */
    property int defaultInsideLeftPage: page.leftPageForCurrent(currentOrFirstContentPage)
    /*!
    */
    property int defaultInsideRightPage: page.rightPageForCurrent(currentOrFirstContentPage)

    /*!
    */
    property alias pageTopMargin: page.pageTopMargin
    /*!
    */
    property alias pageBottomMargin: page.pageBottomMargin
    /*!
    */
    property alias pageGutterMargin: page.pageGutterMargin
    /*!
    */
    property alias pageOuterMargin: page.pageOuterMargin
    /*!
    */
    property alias pageInsideMargin: page.pageInsideMargin

    /*!
    */
    property alias previewTopMargin: page.previewTopMargin
    /*!
    */
    property alias previewBottomMargin: page.previewBottomMargin
    /*!
    */
    property alias previewGutterMargin: page.previewGutterMargin
    /*!
    */
    property alias previewOuterMargin: page.previewOuterMargin
    /*!
    */
    property alias previewInsideMargin: page.previewInsideMargin

    // internal
    /*!
    */
    property int currentOrFirstContentPage: (!album
                                             ? -1
                                             : (album.currentPage == album.firstValidCurrentPage
                                                ? album.firstContentPage
                                                : album.currentPage))

    onAlbumChanged: openFraction = (!album || album.closed ? 0 : 1)

    Connections {
        target: album
        ignoreUnknownSignals: true
        onClosedChanged: openFraction = (album.closed ? 0 : 1)
    }

    Item {
        id: shifter

        x: width * openFraction // Shift it over as it opens so the visuals stay centered.
        y: 0
        width: parent.width
        height: parent.height

        AlbumPageComponent {
            id: page

            anchors.fill: parent

            album: albumViewerTransitionPortraitPage.album

            frontPage: viewingPage
            backPage: album !== null ? album.currentPage : viewingPage

            isPreview: albumOpenerPortrait.isPreview
            contentHasPreviewFrame: albumViewerTransitionPortraitPage.contentHasPreviewFrame

            flipFraction: -openFraction

            topMargin: albumViewerTransitionPortraitPage.topMargin
            bottomMargin: albumViewerTransitionPortraitPage.bottomMargin
            gutterMargin: albumViewerTransitionPortraitPage.gutterMargin
            outerMargin: albumViewerTransitionPortraitPage.outerMargin
            insideMargin: albumViewerTransitionPortraitPage.insideMargin
        }
    }
}

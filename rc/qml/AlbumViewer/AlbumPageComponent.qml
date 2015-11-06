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
import "AlbumInternals"
import "../../js/Gallery.js" as Gallery

// One "piece of paper" in an album.  Contains album content i.e. photos (or
// the cover) from the album and handles rotating, displaying an arbitrary page
// on the front (right) or back (left) sides.
Flipable {
    id: albumPageComponent

    /*!
    */
    property Album album // The album this page is in.

    /*!
    */
    property alias frontPage: frontContents.page // On the right when viewed like in a book.
    /*!
    */
    property alias backPage: backContents.page // On the left when viewed like in a book.

    // [0,2]: 0 = flat right page, 1 = flat left page, 2 = back to right page.
    // The page turns 360 degrees from 0-2 in a normal "book" fashion.
    property real flipFraction: 0

    /*!
    */
    property bool isPreview: false // Whether to load preview or normal images.
    /*!
    */
    property bool load: false
    // These override the isPreview option if specified.
    property alias frontIsPreview: frontContents.isPreview
    /*!
    */
    property alias backIsPreview: backContents.isPreview

    // Whether to draw a frame around the page, if it's a content page.
    property bool contentHasPreviewFrame: false

    /*!
    */
    property real topMargin: pageTopMargin
    /*!
    */
    property real bottomMargin: pageBottomMargin
    /*!
    */
    property real gutterMargin: pageGutterMargin
    /*!
    */
    property real outerMargin: pageOuterMargin
    /*!
    */
    property real insideMargin: pageInsideMargin

    // Prevent the page from reflecting the album as its modified
    property bool freeze: false

    // Whether to draw the cover (defaults to true)
    // This is used to prevent the cover from being drawn during certain transitions.
    property bool showCover: true

    // readonly
    /*!
    */
    property alias frontIsCover: frontContents.isCover
    /*!
    */
    property alias backIsCover: backContents.isCover
    /*!
    */
    property alias frameToContentWidth: frontContents.frameToContentWidth
    /*!
    */
    property alias frameToContentHeight: frontContents.frameToContentHeight
    /*!
    */
    property alias frameHingeInset: frontContents.frameHingeInset

    /*!
    */
    property real pageTopMargin: getDeviceSpecific('albumPageTopMargin')
    /*!
    */
    property real pageBottomMargin: getDeviceSpecific('albumPageBottomMargin')
    /*!
    */
    property real pageGutterMargin: getDeviceSpecific('albumPageGutterMargin')
    /*!
    */
    property real pageOuterMargin: getDeviceSpecific('albumPageOuterMargin')
    /*!
    */
    property real pageInsideMargin: getDeviceSpecific('albumPageInsideMargin')

    /*!
    */
    property real previewTopMargin: getDeviceSpecific('albumPreviewTopMargin')
    /*!
    */
    property real previewBottomMargin: getDeviceSpecific('albumPreviewBottomMargin')
    /*!
    */
    property real previewGutterMargin: getDeviceSpecific('albumPreviewGutterMargin')
    /*!
    */
    property real previewOuterMargin: getDeviceSpecific('albumPreviewOuterMargin')
    /*!
    */
    property real previewInsideMargin: getDeviceSpecific('albumPreviewInsideMargin')

    // Returns the page number of the left page of the current "spread", denoted
    // by a currentPage number from an album.  This will go on the backPage.
    function leftPageForCurrent(currentPage) {
        return currentPage;
    }

    // Returns the page number of the right page of the current "spread", denoted
    // by a currentPage number from an album.  This will go on the frontPage.
    function rightPageForCurrent(currentPage) {
        return currentPage + 1;
    }

    front: AlbumPageContents {
        id: frontContents

        x: isPreview ? -frontContents.frameHingeInset : 0 // origin offset
        width: albumPageComponent.width
        height: albumPageComponent.height

        album: albumPageComponent.album

        isPreview: albumPageComponent.isPreview
        load: albumPageComponent.load
        contentHasPreviewFrame: albumPageComponent.contentHasPreviewFrame

        topMargin: albumPageComponent.topMargin
        bottomMargin: albumPageComponent.bottomMargin
        gutterMargin: albumPageComponent.gutterMargin
        outerMargin: albumPageComponent.outerMargin
        insideMargin: albumPageComponent.insideMargin

        freeze: albumPageComponent.freeze
        showCover: albumPageComponent.showCover
    }

    back: AlbumPageContents {
        id: backContents

        x: isPreview ? frontContents.frameHingeInset : 0 // origin offset
        width: albumPageComponent.width
        height: albumPageComponent.height

        album: albumPageComponent.album

        isPreview: albumPageComponent.isPreview
        load: albumPageComponent.load
        contentHasPreviewFrame: albumPageComponent.contentHasPreviewFrame

        topMargin: albumPageComponent.topMargin
        bottomMargin: albumPageComponent.bottomMargin
        gutterMargin: albumPageComponent.gutterMargin
        outerMargin: albumPageComponent.outerMargin
        insideMargin: albumPageComponent.insideMargin

        freeze: albumPageComponent.freeze
        showCover: albumPageComponent.showCover
    }

    transform: Rotation {
        id: rotation

        origin.x: 0
        origin.y: albumPageComponent.height / 2

        axis.x: 0
        axis.y: 1
        axis.z: 0

        angle: (flipFraction * -180)
    }
}

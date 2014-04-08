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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import ".."

// An item placing the pictures (or cover!) on the front or back of an
// AlbumPageComponent, including an optional frame around the pictures to mimic
// seeing the edge of the back of the cover under the page.
Item {
    id: albumPageContents

    /*!
    */
    property Album album
    /*!
    */
    property int page: -1
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
    property real topMargin
    /*!
    */
    property real bottomMargin
    /*!
    */
    property real gutterMargin
    /*!
    */
    property real outerMargin
    /*!
    */
    property real insideMargin

    // See AlbumPageComponent for descriptions of these properties
    property bool freeze: false
    /*!
    */
    property bool showCover: true

    /*!
    */
    property real frameHingeInset: frame.startX * scaleFactorX

    // readonly
    /*!
    */
    property bool isCover: cover.visible
    // These constants (only useful when contentHasPreviewFrame is true) expose
    // the ratio of size between the whole framed page (displayed at the size of
    // this component) and the content of that page (displayed slightly smaller,
    // within the frame), for if you need to match the contents but not the
    // frame.
    property real frameToContentWidth: ((pixelWidth - 10) / frameContentWidth)
    /*!
    */
    property real frameToContentHeight: ((pixelHeight - 12) / frameContentHeight)

    // internal
    // This might be able to be simplified some, as the components have changed.
    // The original idea was to scale the page up and down, but now the only
    // thing that actually gets scaled is the frame image.
    property bool isRight: (page % 2 == 0)

    // Pixel width of preview frame
    property real pixelWidth: 235
    /*!
    */
    property real pixelHeight: 281

    // Difference between desktop gu size and pixel size.
    property real pixelWidthOffset: 11
    /*!
    */
    property real pixelHeightOffset: 17

    // Offset from frame.start* to the "page" inside the frame.
    property real frameInsetMarginX: isRight ? 0 : -9
    /*!
    */
    property real frameInsetMarginY: 10
    /*!
    */
    property real frameContentOffsetX: frame.startX + frameInsetMarginX
    /*!
    */
    property real frameContentOffsetY: frameInsetMarginY

    // GU/pixel scale factor of preview frame
    property real scaleFactorX: ((isPreview) ? width : (width - gutterMargin)) / (pixelWidth - pixelWidthOffset)
    /*!
    */
    property real scaleFactorY: ((isPreview) ? height : (height - topMargin * 2 - bottomMargin)) / (pixelHeight - pixelHeightOffset)

    // Frame dimensions
    property real frameContentWidth: frame.contentWidth
    /*!
    */
    property real frameContentHeight: frame.contentHeight

    /*!
    */
    property real contentPageX: (contentHasPreviewFrame
                                 ? frameContentOffsetX * scaleFactorX
                                 : 0)
    /*!
    */
    property real contentPageY: (contentHasPreviewFrame
                                 ? frameContentOffsetY * scaleFactorY
                                 : 0)
    /*!
    */
    property real contentPageWidth: (contentHasPreviewFrame
                                     ? frameContentWidth * scaleFactorX
                                     : parent.width)
    /*!
    */
    property real contentPageHeight: (contentHasPreviewFrame
                                      ? frameContentHeight * scaleFactorY
                                      : parent.height)

    /*!
    */
    property variant mediaFrames: (loader.item) ? loader.item.mediaFrames : undefined

    onAlbumChanged: loader.reload()
    onPageChanged: loader.reload()
    onFreezeChanged: {
        if (!freeze)
            loader.reload();
    }

    Connections {
        target: album
        ignoreUnknownSignals: true
        onContentPagesChanged: loader.reload()
    }

    // Album preview page frame.
    Item {
        id: frame

        // Read-only
        // Start of content area
        property int startX: isRight ? frameLeft.width : 18
        property int startY: 6

        // Dimensions of content area
        property int contentWidth: frameContents.width - 10
        property int contentHeight: frameContents.height - 27

        width: pixelWidth
        height: pixelHeight

        transform: Scale {
            xScale: scaleFactorX
            yScale: scaleFactorY
        }

        LayoutMirroring.enabled: !isRight
        LayoutMirroring.childrenInherit: true

        visible: (loader.visible && contentHasPreviewFrame)

        Image {
            id: frameLeft

            source: "img/album-thumbnail-frame-left.png"

            width: 6
            height: pixelHeight
            mirror: !isRight
            anchors.left: parent.left
            anchors.top: parent.top
        }

        Image {
            id: frameContents

            source: "img/album-thumbnail-frame-right.png"

            width: pixelWidth - frameLeft.width
            height: pixelHeight
            mirror: !isRight
            anchors.top: parent.top
            anchors.left: frameLeft.right
        }
    }

    Item {
        id: paperBackground

        anchors.fill: parent

        visible: (loader.visible && !contentHasPreviewFrame && !isPreview)

        clip: true

        Rectangle {
            x: (isRight ? -parent.width : 0)
            y: 0
            width: (isRight ? parent.width * 2 : parent.width)
            height: parent.height
            color: "white"
        }
    }

    Loader {
        id: loader

        // read-only
        property variant mediaSourceList
        property alias topMargin: albumPageContents.topMargin
        property alias bottomMargin: albumPageContents.bottomMargin
        property alias gutterMargin: albumPageContents.gutterMargin
        property alias outerMargin: albumPageContents.outerMargin
        property alias insideMargin: albumPageContents.insideMargin
        property alias isPreview: albumPageContents.isPreview
        property alias load: albumPageContents.load
        property bool isLoaded: false

        function reload() {
            if (freeze || !isLoaded)
                return;

            mediaSourceList = null;
            source = "AlbumPageLayoutLeftPortrait.qml";

            if (!album)
                return;
            var albumPage = album.getPage(page);
            if (!albumPage)
                return;

            mediaSourceList = albumPage.mediaSourceList;
            source = albumPage.qmlRc;
        }

        x: contentPageX
        y: contentPageY
        width: contentPageWidth
        height: contentPageHeight

        visible: (source != null)

        Component.onCompleted: {
            isLoaded = true;
            reload();
        }
    }

    AlbumCover {
        id: cover

        width: pixelWidth
        height: pixelHeight
        y: albumPageContents.isPreview ? 0 : topMargin

        xScale: scaleFactorX
        yScale: scaleFactorY

        visible: showCover && !freeze && (Boolean(album) && (page == 0 || page == album.totalPageCount - 1))

        album: albumPageContents.album
        isBack: !isRight

        // Always consider this as preview, so it matches the preview display when animating out
        // from the cover.
        isPreview: true
    }
}

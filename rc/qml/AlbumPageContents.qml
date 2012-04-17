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

import QtQuick 1.1
import Gallery 1.0

// An item placing the pictures (or cover!) on the front or back of an
// AlbumPageComponent, including an optional frame around the pictures to mimic
// seeing the edge of the back of the cover under the page.
Item {
  id: albumPageContents

  property Album album
  property int page: -1
  property bool isPreview: false
  property bool contentHasPreviewFrame: false

  // readonly
  property bool isCover: cover.visible
  // These constants (only useful when contentHasPreviewFrame is true) expose
  // the ratio of size between the whole framed page (displayed at the size of
  // this component) and the content of that page (displayed slightly smaller,
  // within the frame), for if you need to match the contents but not the
  // frame.
  property real frameToContentWidth: (frameWidth / frameContentWidth)
  property real frameToContentHeight: (frameHeight / frameContentHeight)

  // internal
  property real canonicalWidth: gu(80)
  property real canonicalHeight: gu(97)
  property real canonicalTopMargin: gu(6)
  property real canonicalBottomMargin: gu(6)
  property real canonicalGutterMargin: gu(2) // Between spine and photo.
  property real canonicalOuterMargin: gu(3) // Between opposite edge and photo.
  property real canonicalInsideMargin: gu(4) // Between photos on one page.

  // Apologies for how complex this got.  The idea was to have one item that
  // would scale down from some reference size easily so it would look good no
  // matter what size you gave it.  Unfortunately, the frame image was given
  // to us at thumbnail size, so we're scaling it up while we scale other bits
  // down.  That makes this component somewhat complicated, but greatly
  // simplifies everything that happens above it.
  property bool isRight: (page % 2 == 0)
  // Where the transparent shadow ends in the frame image.
  property real frameStartX: (isRight ? 6 : 5)
  property real frameStartY: 5
  property real frameWidth: gu(28)
  property real frameHeight: gu(33)
  // Offset from frameStart* to the "page" inside the frame.
  property real frameContentOffsetX: (isRight ? 0 : 5)
  property real frameContentOffsetY: 5
  property real frameContentWidth: 219
  property real frameContentHeight: 254
  property real contentPageX: (contentHasPreviewFrame
    ? frameContentOffsetX * (parent.width / frameWidth)
    : 0)
  property real contentPageY: (contentHasPreviewFrame
    ? frameContentOffsetY * (parent.height / frameHeight)
    : 0)
  property real canonicalContentPageWidth: (contentHasPreviewFrame
    ? frameContentWidth * (canonicalWidth / frameWidth)
    : canonicalWidth)
  property real canonicalContentPageHeight: (contentHasPreviewFrame
    ? frameContentHeight * (canonicalHeight / frameHeight)
    : canonicalHeight)
  
  property variant mediaFrames: (loader.item) ? loader.item.mediaFrames : undefined
  
  onAlbumChanged: loader.reload()
  onPageChanged: loader.reload()

  Connections {
    target: album
    ignoreUnknownSignals: true
    onContentPagesAltered: loader.reload()
  }

  Image {
    id: frame

    x: -frameStartX
    y: -frameStartY

    transform: Scale {
      origin.x: frameStartX
      origin.y: frameStartY
      xScale: parent.width / frameWidth
      yScale: parent.height / frameHeight
    }

    source: "../img/album-thumbnail-frame.png"
    cache: true
    mirror: !isRight
    visible: (loader.visible && contentHasPreviewFrame)
  }

  Rectangle {
    id: nonframeBackground

    width: canonicalWidth
    height: canonicalHeight

    transform: Scale {
      xScale: parent.width / canonicalWidth
      yScale: parent.height / canonicalHeight
    }

    visible: (loader.visible && !contentHasPreviewFrame)
  }

  Loader {
    id: loader

    // read-only
    property variant mediaSourceList
    property alias topMargin: albumPageContents.canonicalTopMargin
    property alias bottomMargin: albumPageContents.canonicalBottomMargin
    property alias gutterMargin: albumPageContents.canonicalGutterMargin
    property alias outerMargin: albumPageContents.canonicalOuterMargin
    property alias insideMargin: albumPageContents.canonicalInsideMargin
    property alias isPreview: albumPageContents.isPreview

    function reload() {
      mediaSourceList = null;
      source = "";

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
    width: canonicalContentPageWidth
    height: canonicalContentPageHeight

    visible: (source != "")

    transform: Scale {
      xScale: parent.width / canonicalWidth
      yScale: parent.height / canonicalHeight
    }

    Component.onCompleted: reload()
  }

  AlbumCover {
    id: cover

    anchors.fill: parent

    visible: (Boolean(album) && (page == 0 || page == album.totalPageCount - 1))

    album: albumPageContents.album
    isBack: !isRight
  }
}

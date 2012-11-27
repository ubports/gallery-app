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

  property Album album
  property int page: -1
  property bool isPreview: false
  property bool contentHasPreviewFrame: false

  property real topMargin
  property real bottomMargin
  property real gutterMargin
  property real outerMargin
  property real insideMargin
  
  // See AlbumPageComponent for descriptions of these properties
  property bool freeze: false
  property bool showCover: true
  
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
  // This might be able to be simplified some, as the components have changed.
  // The original idea was to scale the page up and down, but now the only
  // thing that actually gets scaled is the frame image.
  property bool isRight: (page % 2 == 0)
  // Where the transparent shadow ends in the frame image.
  property real frameStartX: (isRight ? 6 : 5)
  property real frameStartY: 5
  property real frameWidth: units.gu(28)
  property real frameHeight: units.gu(33)
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
  property real contentPageWidth: (contentHasPreviewFrame
    ? frameContentWidth * (parent.width / frameWidth)
    : parent.width)
  property real contentPageHeight: (contentHasPreviewFrame
    ? frameContentHeight * (parent.height / frameHeight)
    : parent.height)
  
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
    onContentPagesAltered: loader.reload()
  }
  
  Image {
    id: frame

    x: -frameStartX
    y: -frameStartY

    transform: Scale {
      origin.x: frameStartX
      origin.y: frameStartY
      xScale: albumPageContents.width / frameWidth
      yScale: albumPageContents.height / frameHeight
    }

    source: "img/album-thumbnail-frame.png"
    mirror: !isRight
    visible: (loader.visible && contentHasPreviewFrame)
  }

  Rectangle {
    id: plainBackground

    anchors.fill: parent

    visible: (loader.visible && !contentHasPreviewFrame && isPreview)
  }

  Item {
    id: paperBackground

    anchors.fill: parent

    visible: (loader.visible && !contentHasPreviewFrame && !isPreview)

    clip: true

    Image {
      x: (isRight ? -parent.width : 0)
      y: 0
      width: (isRight ? parent.width * 2 : parent.width)
      height: parent.height

      source: "../../../img/background-paper.png"

      fillMode: Image.Tile
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

    anchors.fill: parent

    visible: showCover && !freeze && (Boolean(album) && (page == 0 || page == album.totalPageCount - 1))

    album: albumPageContents.album
    isBack: !isRight
    isPreview: albumPageContents.isPreview
  }
}

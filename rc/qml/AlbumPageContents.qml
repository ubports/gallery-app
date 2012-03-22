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

// A rectangle placing the pictures on the front or back of an
// AlbumPageComponent.  Note: this component draws one side of one "piece of
// paper", that is, half a spread (half an AlbumPage in the C++ sense).
Rectangle {
  id: albumPageContents

  property Album album
  property bool isCover: false
  property AlbumPage albumPage
  property bool isLeft: false
  property bool isPreview: false

  // readonly
  property real canonicalWidth: gu(80)
  property real canonicalHeight: gu(97)
  property real canonicalTopMargin: gu(6)
  property real canonicalBottomMargin: gu(6)
  property real canonicalGutterMargin: gu(2) // Between spine and photo.
  property real canonicalOuterMargin: gu(3) // Between opposite edge and photo.
  property real canonicalInsideMargin: gu(4) // Between photos on one page.

  // internal
  property real aspect: canonicalWidth / canonicalHeight

  onAlbumPageChanged: {
    if (!albumPage || isCover)
      loader.source = "";
    else
      loader.source = (isLeft ? albumPage.leftQmlRc : albumPage.rightQmlRc);
  }

  Loader {
    id: loader

    // read-only
    property variant mediaSourceList: (albumPage) ? albumPage.mediaSourceList : null
    property alias topMargin: albumPageContents.canonicalTopMargin
    property alias bottomMargin: albumPageContents.canonicalBottomMargin
    property alias gutterMargin: albumPageContents.canonicalGutterMargin
    property alias outerMargin: albumPageContents.canonicalOuterMargin
    property alias insideMargin: albumPageContents.canonicalInsideMargin
    property alias isPreview: albumPageContents.isPreview

    width: canonicalWidth
    height: canonicalHeight

    transformOrigin: Item.TopLeft
    scale: ((parent.width / parent.height > canonicalWidth / canonicalHeight)
      ? parent.height / canonicalHeight // Wider than canonical, scale up to height only.
      : parent.width / canonicalWidth) // Higher than canonical, scale up to width only.

    visible: (source != "")

    source: {
      if (!albumPage || isCover)
        return "";
      return (isLeft ? albumPage.leftQmlRc : albumPage.rightQmlRc);
    }

    onMediaSourceListChanged: {
      if (!mediaSourceList || source == "")
        return;

      // MediaSources within page have changed, force reload of same QML file
      var src = source;
      source = "";
      source = src;
    }
  }

  AlbumCover {
    id: cover

    width: canonicalWidth
    height: canonicalHeight

    transformOrigin: Item.TopLeft
    scale: ((parent.width / parent.height > canonicalWidth / canonicalHeight)
      ? parent.height / canonicalHeight // Wider than canonical, scale up to height only.
      : parent.width / canonicalWidth) // Higher than canonical, scale up to width only.

    visible: (!!album && isCover)

    album: albumPageContents.album
  }
}

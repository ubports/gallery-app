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

// A specialized component to animate an album closing via the last page being
// turned.
// TODO: there's a lot of duplication between this and AlbumOpener.
Item {
  id: albumCloser

  // public
  property Album album
  property bool isPreview: false
  property bool contentHasPreviewFrame: false
  property int duration: 1000
  property alias leftFlipFraction: leftPage.flipFraction
  property alias rightFlipFraction: rightPage.flipFraction

  // readonly
  property alias frameToContentWidth: rightPage.frameToContentWidth
  property alias frameToContentHeight: rightPage.frameToContentHeight

  // internal
  property int currentPage: (!album ? -1 : album.currentPage)

  function close() {
    rightAnimator.restart();
    leftAnimator.restart();
  }

  Item {
    id: shifter

    // Shift it over as it opens so the visuals stay centered.
    x: width * leftFlipFraction * 0.5
    y: 0
    width: parent.width
    height: parent.height

    AlbumPageComponent {
      id: rightPage

      anchors.fill: parent
      visible: leftFlipFraction > 0

      album: albumCloser.album
      frontPage: rightPageForCurrent(currentPage)
      backPage: (album ? leftPageForCurrent(album.lastValidCurrentPage) : -1)

      isPreview: albumCloser.isPreview
      contentHasPreviewFrame: albumCloser.contentHasPreviewFrame

      NumberAnimation {
        id: rightAnimator

        target: rightPage
        property: "flipFraction"
        to: 0
        easing.type: Easing.OutQuad
        duration: albumCloser.duration
      }
    }

    AlbumPageComponent {
      id: leftPage

      anchors.fill: parent

      album: albumCloser.album
      frontPage: (album ? rightPageForCurrent(album.firstValidCurrentPage) : -1)
      backPage: leftPageForCurrent(currentPage)

      isPreview: albumCloser.isPreview
      contentHasPreviewFrame: albumCloser.contentHasPreviewFrame

      NumberAnimation {
        id: leftAnimator

        target: leftPage
        property: "flipFraction"
        to: 0
        easing.type: Easing.OutQuad
        duration: albumCloser.duration

        onCompleted: album.closed = true
      }
    }
  }
}

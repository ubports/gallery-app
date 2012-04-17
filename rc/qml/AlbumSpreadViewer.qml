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
 * Jim Nelson <jim@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 *
 * Originally inspired by code written by Tommi Laukkanen, available at
 * http://www.substanceofcode.com/downloads/flipboard_page_turn_with_qml.zip
 */

import QtQuick 1.1
import Gallery 1.0
import "GalleryUtility.js" as GalleryUtility

// The AlbumSpreadViewer is a specialty controller for displaying and
// viewing albums.  The caller simply needs to set the album property and
// then manipulate what is displayed via turnTo() (which animates the page
// flipping as though turning pages in a book), setTo() (which immediately
// jumps to the page without an animation).  To perform incremental flipping,
// first set turnTowardPage (which will become the album's
// currentPage) and manipulate turnFraction on the range [0, 1].
//
// AlbumSpreadViewer updates the album object at the end of the animation,
// meaning the turned-to page is treated as the current album page throughout
// the system.  This is only for turnTo(); setTo() does not update album page
// state.
Item {
  id: albumSpreadViewer
  
  signal pageReleased()
  signal pageFlipped()
  
  // public
  property Album album
  property bool isPreview: false
  property bool contentHasPreviewFrame: false
  property int durationMsec: 1000
  property int turnTowardPage: 0
  property real turnFraction: 0.0
  
  // readonly
  property bool isRunning: (animatedPage.state != "")
  property alias isFlipping: animatedPage.visible
  property real currentFraction: (leftToRight
    ? animatedPage.flipFraction
    : 1 - animatedPage.flipFraction)
  property alias rightIsCover: right.frontIsCover
  
  // internal
  property alias leftPage: left.backPage
  property alias rightPage: right.frontPage
  property alias animatedFrontPage: animatedPage.frontPage
  property alias animatedBackPage: animatedPage.backPage
  // leftToRight = true when the animated page is moving from the right side to
  // the left, like flipping a page forward in an LTR language.
  property bool leftToRight: true
  property int destAlbumCurrentPage
  property real rotationOrigin: leftToRight ? 0 : 1
  property real rotationDestination: leftToRight ? 1 : 0
  property real startFraction: 0
  property real endFraction: 1
  property real startRotation: (leftToRight ? startFraction : 1 - startFraction)
  property real endRotation: (leftToRight ? endFraction : 1 - endFraction)
  
  // Converts a page number into the appropriate page number to place on the
  // left-hand side of the component
  function getLeftHandPageNumber(pageNumber) {
    if (pageNumber <= album.firstValidCurrentPage)
      return album.firstValidCurrentPage;
    
    if (pageNumber >= album.lastValidCurrentPage)
      return album.lastValidCurrentPage;
    
    return GalleryUtility.isOdd(pageNumber) ? pageNumber : pageNumber - 1;
  }
  
  // Moves to selected page without any animation (good for initializing view).
  // NOTE: setTo() does *not* update the album's current page number.
  function setTo(newCurrentPage) {
    if (isRunning) {
      console.log("Blocking set of album page", newCurrentPage);
      return;
    }
    
    // Because setTo() doesn't control the album's state, only check for
    // bounds issue, not setting to the same page number (which this component
    // may not be showing currently)
    if (!album || newCurrentPage < album.firstValidCurrentPage
      || newCurrentPage > album.lastValidCurrentPage)
      return;

    leftPage = left.leftPageForCurrent(newCurrentPage);
    rightPage = right.rightPageForCurrent(newCurrentPage);
    animatedFrontPage = -1;
    animatedBackPage = -1;
  }
  
  // Calls either setTo() with the album's current page.  An easy way to keep
  // this state consistent with the album's.
  function setToAlbumCurrent() {
    if (album)
      setTo(album.currentPage);
  }
  
  // Animates page flip and saves new page number to album.
  function turnTo(newCurrentPage) {
    if (newCurrentPage < album.firstValidCurrentPage
      || newCurrentPage > album.lastValidCurrentPage) {
      releasePage();
      return;
    }
    
    if (!prepareToTurn(newCurrentPage))
      return;
    
    startFraction = currentFraction;
    endFraction = 1.0;
    
    start(true);
  }
  
  // Call when done incrementally animating to animate the return to a static
  // state.
  function releasePage() {
    if (isRunning) {
      console.log("Blocking release of page");
      return;
    }

    if (!album)
      return;

    startFraction = currentFraction;
    endFraction = 0.0;
    destAlbumCurrentPage = album.currentPage;

    animatedPage.state = "animateAutomatic";
  }

  onTurnFractionChanged: {
    if (!prepareToTurn(turnTowardPage))
      return;
    
    startFraction = currentFraction;
    endFraction = turnFraction;
    
    start(false);
  }
  
  // internal
  function prepareToTurn(newCurrentPage) {
    if (isRunning) {
      console.log("Blocking turn to album page", newCurrentPage);
      return false;
    }
    
    if (!album || newCurrentPage < album.firstValidCurrentPage
      || newCurrentPage > album.lastValidCurrentPage
      || newCurrentPage == album.currentPage) {
      return false;
    }
    
    var oldLeftToRight = leftToRight;
    if (album.currentPage > newCurrentPage) {
      leftPage = left.leftPageForCurrent(newCurrentPage);
      animatedFrontPage = animatedPage.rightPageForCurrent(newCurrentPage);
      animatedBackPage = animatedPage.leftPageForCurrent(album.currentPage);
      leftToRight = false;
    } else {
      // album.currentPage < newCurrentPage
      rightPage = right.rightPageForCurrent(newCurrentPage);
      animatedFrontPage = animatedPage.rightPageForCurrent(album.currentPage);
      animatedBackPage = animatedPage.leftPageForCurrent(newCurrentPage);
      leftToRight = true;
    }

    // initialize rotation angle for changed direction
    if (oldLeftToRight != leftToRight)
      animatedPage.flipFraction = rotationOrigin;
    
    destAlbumCurrentPage = newCurrentPage;

    return true;
  }
  
  // internal
  function start(automatic) {
    if (isRunning) {
      console.log("Blocking start of page flip animation");
      return;
    }
    
    if (automatic) {
      if (animatedPage.flipFraction == rotationDestination)
        animatedPage.rotationCompleted();
      else
        animatedPage.state = "animateAutomatic";
    } else if (endFraction <= 0.0) {
      animatedPage.flipFraction = rotationOrigin;
    } else if (endFraction >= 1.0) {
      animatedPage.flipFraction = rotationDestination;
    } else {
      // 0.0 < endFraction < 1.0; endRotation calculates the appropriate angle
      animatedPage.flipFraction = endRotation;
    }
  }
  
  // public
  function hitTestFrame(x, y, relativeTo) {
    // current visible photos are on the back of the left page and the front
    // of the right page
    var hit = hitTestPage(left.back, x, y, relativeTo);
    if (hit)
      return hit;
    
    return hitTestPage(right.front, x, y, relativeTo);
  }
  
  // internal
  function hitTestPage(page, x, y, relativeTo) {
    if (!page.mediaFrames)
      return undefined;
    
    var ctr;
    for (ctr = 0; ctr < page.mediaFrames.length; ctr++) {
      var rect = GalleryUtility.getRectRelativeTo(page.mediaFrames[ctr], relativeTo);
      if (GalleryUtility.doesPointIntersectRect(x, y, rect))
        return page.mediaFrames[ctr];
    }
    
    return undefined;
  }
  
  // public
  function getRectOfMediaSource(media) {
    // current visible photos are on the back of the left page and the front of
    // the right page
    var rect = searchPageForMedia(left.back, media);
    if (rect)
      return rect;
    
    return searchPageForMedia(right.front, media);
  }
  
  // private
  function searchPageForMedia(page, media) {
    if (!page.mediaFrames)
      return undefined;
    
    var ctr;
    for (ctr = 0; ctr < page.mediaFrames.length; ctr++) {
      if (page.mediaFrames[ctr].mediaSource && page.mediaFrames[ctr].mediaSource.equals(media))
        return page.mediaFrames[ctr];
    }
    
    return undefined;
  }
  
  onAlbumChanged: setToAlbumCurrent()

  onVisibleChanged: setToAlbumCurrent()
  
  AlbumPageComponent {
    id: left

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    visible: (backPage >= 0)

    album: albumSpreadViewer.album
    isPreview: albumSpreadViewer.isPreview
    contentHasPreviewFrame: albumSpreadViewer.contentHasPreviewFrame

    flipFraction: 1
  }

  AlbumPageComponent {
    id: right

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    visible: (Boolean(album) && frontPage < album.totalPageCount)

    album: albumSpreadViewer.album
    isPreview: albumSpreadViewer.isPreview
    contentHasPreviewFrame: albumSpreadViewer.contentHasPreviewFrame

    flipFraction: 0
  }

  AlbumPageComponent {
    id: animatedPage

    // internal
    // Called when a page has completely flipped (rotation is completed)
    function rotationCompleted() {
      var flipped = false;
      if (album.currentPage != destAlbumCurrentPage) {
        flipped = true;
        album.currentPage = destAlbumCurrentPage;
        if (destAlbumCurrentPage == album.firstValidCurrentPage
          || destAlbumCurrentPage == album.lastValidCurrentPage)
          album.closed = true;
        else
          album.closed = false;
      }

      state = "";

      setToAlbumCurrent();

      // notify subscribers
      if (flipped)
        pageFlipped();
      else
        pageReleased();
    }

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    visible: (Boolean(album) && (
      (frontPage >= 0 && frontPage < album.totalPageCount)
      || (backPage >= 0 && backPage < album.totalPageCount)
    ))

    album: albumSpreadViewer.album
    isPreview: albumSpreadViewer.isPreview
    contentHasPreviewFrame: albumSpreadViewer.contentHasPreviewFrame

    states: State {
      name: "animateAutomatic"

      PropertyChanges {
        target: animatedPage
        flipFraction: endRotation
      }
    }

    transitions: Transition {
      to: "animateAutomatic"
      reversible: false

      SequentialAnimation {
        NumberAnimation {
          target: animatedPage
          property: "flipFraction"
          duration: durationMsec
          easing.type: Easing.OutQuad
        }

        ScriptAction {
          script: {
            if (animatedPage.flipFraction == rotationDestination
              || animatedPage.flipFraction == rotationOrigin)
              animatedPage.rotationCompleted();
            else
              animatedPage.state = "";
          }
        }
      }
    }
  }
}

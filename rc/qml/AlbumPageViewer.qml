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

// The AlbumPageViewer is a specialty controller for displaying and
// viewing albums.  The caller simply needs to set the album property and
// then manipulate what is displayed via turnTo() (which animates the page
// flipping as though turning pages in a book), setTo() (which immediately
// jumps to the page without an animation).  To perform incremental flipping,
// first set turnTowardPageNumber (which will become the album's
// currentPageNumber) and manipulate turnFraction on the range [0, 1].
//
// AlbumPageViewer updates the album object at the end of the animation,
// meaning the turned-to page is treated as the current album page throughout
// the system.  This is only for turnTo(); setTo() does not update album page
// state.
Item {
  id: albumPageViewer
  
  signal pageReleased()
  signal pageFlipped()
  
  // public
  property Album album
  property bool isPreview: false
  property int durationMsec: 1000
  property int turnTowardPageNumber: 0
  property real turnFraction: 0.0
  
  // readonly
  property bool isRunning: (animatedPage.state != "")
  property real currentFraction: (leftToRight ? animatedPage.flipFraction : 1 - animatedPage.flipFraction)
  
  // internal
  property alias leftPageNumber: leftPage.backPageNumber
  property alias rightPageNumber: rightPage.frontPageNumber
  property alias animatedFrontPageNumber: animatedPage.frontPageNumber
  property alias animatedBackPageNumber: animatedPage.backPageNumber
  // leftToRight = true when the animated page is moving from the right side to
  // the left, like flipping a page forward in an LTR language.
  property bool leftToRight: true
  property int destAlbumCurrentPageNumber
  property real rotationOrigin: leftToRight ? 0 : 1
  property real rotationDestination: leftToRight ? 1 : 0
  property real startFraction: 0
  property real endFraction: 1
  property real startRotation: (leftToRight ? startFraction : 1 - startFraction)
  property real endRotation: (leftToRight ? endFraction : 1 - endFraction)
  
  // Moves to selected page without any animation (good for initializing view).
  // NOTE: setTo() does *not* update the album's current page number.
  function setTo(index) {
    if (isRunning) {
      console.log("Blocking set of album page", index);
      return;
    }
    
    // Because setTo() doesn't control the album's state, only check for
    // bounds issue, not setting to the same page number (which this component
    // may not be showing currently)
    if (!album || index < album.firstCurrentPageNumber || index > album.lastCurrentPageNumber)
      return;

    leftPageNumber = index;
    rightPageNumber = index + 1;
    animatedFrontPageNumber = -1;
    animatedBackPageNumber = -1;
  }
  
  // Calls either setTo() with the album's current page.  An easy way to keep
  // this state consistent with the album's.
  function setToAlbumCurrent() {
    if (album)
      setTo(album.currentPageNumber);
  }
  
  // Animates page flip and saves new page number to album.
  function turnTo(index) {
    if (index < album.firstCurrentPageNumber || index > album.lastCurrentPageNumber) {
      releasePage();
      return;
    }
    
    if (!prepareToTurn(index))
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
    destAlbumCurrentPageNumber = album.currentPageNumber;

    animatedPage.state = "animateAutomatic";
  }

  onTurnFractionChanged: {
    if (!prepareToTurn(turnTowardPageNumber))
      return;
    
    startFraction = currentFraction;
    endFraction = turnFraction;
    
    start(false);
  }
  
  // internal
  function prepareToTurn(index) {
    if (isRunning) {
      console.log("Blocking turn to album page", index);
      return false;
    }
    
    if (!album || index < album.firstCurrentPageNumber || index > album.lastCurrentPageNumber
      || index == album.currentPageNumber) {
      return false;
    }
    
    var oldLeftToRight = leftToRight;
    if (album.currentPageNumber > index) {
      leftPageNumber = index;
      animatedFrontPageNumber = index + 1;
      animatedBackPageNumber = album.currentPageNumber;
      leftToRight = false;
    } else {
      // album.currentPageNumber < index
      rightPageNumber = index + 1;
      animatedFrontPageNumber = album.currentPageNumber + 1;
      animatedBackPageNumber = index;
      leftToRight = true;
    }

    // initialize rotation angle for changed direction
    if (oldLeftToRight != leftToRight)
      animatedPage.flipFraction = rotationOrigin;
    
    destAlbumCurrentPageNumber = index;

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
  
  onAlbumChanged: setToAlbumCurrent()

  onVisibleChanged: setToAlbumCurrent()
  
  AlbumPageComponent {
    id: leftPage

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    visible: (backPageNumber >= 0)

    album: albumPageViewer.album
    isPreview: albumPageViewer.isPreview

    flipFraction: 1
  }

  AlbumPageComponent {
    id: rightPage

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    visible: (!!album && frontPageNumber < album.totalPageCount)

    album: albumPageViewer.album
    isPreview: albumPageViewer.isPreview

    flipFraction: 0
  }

  AlbumPageComponent {
    id: animatedPage

    // internal
    // Called when a page has completely flipped (rotation is completed)
    function rotationCompleted() {
      var flipped = false;
      if (album.currentPageNumber != destAlbumCurrentPageNumber) {
        flipped = true;
        album.currentPageNumber = destAlbumCurrentPageNumber;
        if (destAlbumCurrentPageNumber == album.firstCurrentPageNumber
          || destAlbumCurrentPageNumber == album.lastCurrentPageNumber)
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

    visible: (!!album && (
      (frontPageNumber >= 0 && frontPageNumber < album.totalPageCount)
      || (backPageNumber >= 0 && backPageNumber < album.totalPageCount)
    ))

    album: albumPageViewer.album
    isPreview: albumPageViewer.isPreview

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
            if (animatedPage.flipFraction == rotationDestination || animatedPage.flipFraction == rotationOrigin)
              animatedPage.rotationCompleted();
            else
              animatedPage.state = "";
          }
        }
      }
    }
  }
}

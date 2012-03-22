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
// then manipulate what is displayed via turnTo()/close() (which animate the
// page flipping as though turning pages in a book), setTo()/setToClosed()
// (which immediately jump to the page without an animation).
//
// AlbumPageViewer updates the album object at the end of the animation,
// meaning the final page is treated as the current album page throughout
// the system.  This is only for turnTo() and close(); setTo() and
// setToClosed() does not update album page state.
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
  property int leftPageNumber // In AlbumPageComponent terms.
  property int rightPageNumber // In AlbumPageComponent terms.
  property int animatedPageNumber // In AlbumPageComponent terms.
  // leftToRight = true when the animated page is moving from the right side to
  // the left, like flipping a page forward in an LTR language.
  property bool leftToRight: true
  property int destAlbumPageNumber // In C++ Album terms.
  property bool destAlbumClosed
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
    if (!album || index < 0 || index >= album.pageCount)
      return;

    leftPageNumber = index;
    rightPageNumber = index + 1;
    animatedPageNumber = -1;
  }
  
  // Moves to album cover without any animation (good for initializing view).
  // NOTE: setToClosed() does *not* update the album's state.
  function setToClosed() {
    if (isRunning) {
      console.log("Blocking set of album page to closed");
      return;
    }
    
    leftPageNumber = -1;
    rightPageNumber = 0;
    animatedPageNumber = -1;
  }

  // Calls either setTo() or setToClosed() depending on the current state of
  // the album.  An easy way to keep this state consistent with the album's.
  function setToAlbumCurrent() {
    if (album) {
      if (album.closed)
        setToClosed()
      else
        setTo(album.currentPageNumber);
    }
  }
  
  // Animates page flip and saves new page number to album.
  function turnTo(index) {
    if (index >= album.pageCount) {
      releasePage();
      return;
    }
    
    if (!prepareToTurn(index))
      return;
    
    startFraction = currentFraction;
    endFraction = 1.0;
    
    start(true);
  }
  
  function close() {
    if (!prepareToClose())
      return;

    startFraction = currentFraction;
    endFraction = 1.0;

    start(true);
  }

  function releasePage() {
    if (isRunning) {
      console.log("Blocking release of page");
      return;
    }

    if (!album)
      return;

    startFraction = currentFraction;
    endFraction = 0.0;
    destAlbumPageNumber = album.currentPageNumber;
    destAlbumClosed = album.closed;

    animatedPage.state = "animateAutomatic";
  }

  onTurnFractionChanged: {
    if (!prepareToTurn(turnTowardPageNumber))
      return;
    
    startFraction = currentFraction;
    endFraction = turnFraction;
    
    start(false);
  }
  
  function turnTowardCover(fraction) {
    if (!prepareToClose())
      return;
    
    startFraction = currentFraction;
    endFraction = fraction;
    
    start(false);
  }
  
  // internal
  function prepareToClose() {
    if (isRunning) {
      console.log("Blocking close of album");
      return false;
    }
    
    if (!album || album.closed)
      return false;
    
    var oldLeftToRight = leftToRight;
    leftToRight = false;

    leftPageNumber = -1;
    animatedPageNumber = 0;

    destAlbumPageNumber = album.currentPageNumber;
    destAlbumClosed = true;
    
    // initialize rotation angle for changed direction
    if (oldLeftToRight != leftToRight)
      animatedPage.flipFraction = rotationOrigin;

    return true;
  }
  
  // internal
  function prepareToTurn(index) {
    if (isRunning) {
      console.log("Blocking turn to album page", index);
      return false;
    }
    
    // Can turn past the last page, but only pull it up, not close the album
    // with the back cover facing the user
    if (!album || (index >= album.pageCount + 1)
      || (index == album.currentPageNumber && !album.closed)) {
      return false;
    }
    
    var oldLeftToRight = leftToRight;
    if (album.closed) {
      leftPageNumber = -1;
      rightPageNumber = index + 1;
      animatedPageNumber = index;
      leftToRight = true;
    } else if (album.currentPageNumber > index) {
      leftPageNumber = index;
      rightPageNumber = album.currentPageNumber + 1;
      animatedPageNumber = index + 1;
      leftToRight = false;
    } else {
      // album.currentPageNumber < index
      leftPageNumber = album.currentPageNumber;
      rightPageNumber = index + 1;
      animatedPageNumber = index;
      leftToRight = true;
    }

    // initialize rotation angle for changed direction
    if (oldLeftToRight != leftToRight)
      animatedPage.flipFraction = rotationOrigin;
    
    destAlbumPageNumber = index;
    destAlbumClosed = false;

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

    visible: leftPageNumber >= 0

    album: albumPageViewer.album
    isPreview: albumPageViewer.isPreview

    pageNumber: leftPageNumber
    flipFraction: 1
  }

  AlbumPageComponent {
    id: rightPage

    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right

    visible: rightPageNumber >= 0

    album: albumPageViewer.album
    isPreview: albumPageViewer.isPreview

    pageNumber: rightPageNumber
    flipFraction: 0
  }

  AlbumPageComponent {
    id: animatedPage

    // internal
    // Called when a page has completely flipped (rotation is completed)
    function rotationCompleted() {
      var flipped = false;
      if (destAlbumClosed) {
        if (!album.closed) {
          flipped = true;
          album.closed = true;
        }
      } else {
        if (album.currentPageNumber != destAlbumPageNumber) {
          flipped = true;
          album.currentPageNumber = destAlbumPageNumber;
        }
        if (album.closed) {
          flipped = true;
          album.closed = false;
        }
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

    visible: animatedPageNumber >= 0

    album: albumPageViewer.album
    isPreview: albumPageViewer.isPreview

    pageNumber: animatedPageNumber

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

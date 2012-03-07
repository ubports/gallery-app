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
 *
 * Inspired by code written by Tommi Laukkanen, available at
 * http://www.substanceofcode.com/downloads/flipboard_page_turn_with_qml.zip
 */

import QtQuick 1.1
import Gallery 1.0

// The AlbumPageViewer is a specialty controller for displaying and
// viewing albums.  The caller simply needs to set the album property and
// then manipulate what is displayed via turnTo() (which animates the page
// flipping as though turning pages in a book), setTo() (which immediately
// jumps to the page without an animation), or close() which animates closing
// the album; there is no no-animation variant at this time).
//
// AlbumPageViewer updates the album object at the end of the animation,
// meaning the final page is treated as the current album page throughout
// the system.  This is only for turnTo() and close(); setTo() does not update
// album page state.
//
// The animation works by using a Flipable component with the two pages on
// its front and back along with halves of the components making up the
// background (thus, two of each component is instantiated).  A sliding clipping
// region moves with the animation, displaying one half of the component as
// the background and the other half in animation, to simulate the book turning
// on its spine.
//
// When not animating, AlbumPageViewer enters a state where the two
// halves are shown side-by-side, making it look like a single page.  This may
// seem unnecessary, but it removes the need for a separate component to show
// the entire page and then showing/hiding it via z-order, visibility, or some
// other trick.
Item {
  id: albumPageViewer
  
  signal pageReleased()
  
  // public
  property Album album
  property int durationMsec: 1000
  property int turnTowardPageNumber: 0
  property real turnFraction: 0.0
  property bool isPreview: false
  property real topGutter: gu(6)
  property real bottomGutter: gu(6)
  property real leftGutter: gu(3)
  property real rightGutter: gu(3)
  property real spineGutter: gu(2)
  property real insideGutter: gu(2)
  property real coverTitleOpacity: 1
  
  // readonly
  property bool pageFlipped: false
  property bool isRunning: (flippable.state != "" || clipper.triggerAutomatic)
  property real currentFraction: leftToRight ? (rotationTransform.angle / -180.0)
    : 1.0 - (rotationTransform.angle / -180.0)
  
  // internal
  property AlbumPage leftPage
  property AlbumPage rightPage
  property bool leftToRight: true
  property bool leftIsCover: false
  property bool rightIsCover: false
  property bool rightIsBackCover: false
  property int finalPageNumber
  property bool finalPageIsCover
  property real rotationOrigin: leftToRight ? 0.0 : -180.0
  property real rotationDestination: leftToRight ? -180.0 : 0.0
  property real startFraction: 0.0
  property real endFraction: 1.0
  property real startRotation: leftToRight ? (-180.0 * startFraction) : -180.0 - (-180.0 * startFraction)
  property real endRotation: leftToRight ? (-180.0 * endFraction) : -180.0 - (-180.0 * endFraction)
  
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

    leftIsCover = false;
    rightIsCover = false;
    leftPage = album.getPage(index);
    rightPage = leftPage;

    clipper.visible = false;
  }
  
  // Moves to album cover without any animation (good for initializing view).
  // NOTE: setToClosed() does *not* update the album's state.
  function setToClosed() {
    if (isRunning) {
      console.log("Blocking set of album page", index);
      
      return;
    }
    
    leftIsCover = true;
    rightIsCover = true;
    
    clipper.visible = false;
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
  
  onTurnFractionChanged: {
    if (!prepareToTurn(turnTowardPageNumber))
      return;
    
    startFraction = currentFraction;
    endFraction = turnFraction;
    
    start(false);
  }
  
  function releasePage() {
    if (isRunning) {
      console.log("Blocking release of page");
      
      return;
    }
    
    if (!album)
      return;
    
    clipper.visible = true;
    
    pageFlipped = false;
    
    startFraction = currentFraction;
    endFraction = 0.0;
    finalPageNumber = album.currentPageNumber;
    finalPageIsCover = album.closed;
    
    clipper.triggerAutomatic = true;
  }
  
  function close() {
    if (!prepareToClose())
      return;
    
    startFraction = currentFraction;
    endFraction = 1.0;
    
    start(true);
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
    
    leftIsCover = true;
    rightIsCover = false;
    rightPage = album.currentPage;
    leftToRight = false;
    finalPageIsCover = true;
    
    // initialize rotation angle for changed direction
    if (oldLeftToRight != leftToRight)
      rotationTransform.angle = rotationOrigin;

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
    
    leftIsCover = false;
    rightIsCover = false;
    
    var oldLeftToRight = leftToRight;
    if (album.closed) {
      leftIsCover = true;
      rightPage = album.getPage(index);
      leftToRight = true;
    } else if (album.currentPageNumber > index) {
      leftPage = album.getPage(index);
      rightPage = album.getPage(album.currentPageNumber);
      leftToRight = false;
    } else {
      // album.currentPageNumber < index
      leftPage = album.getPage(album.currentPageNumber);
      leftToRight = true;
      if (index < album.pageCount) {
        rightPage = album.getPage(index);
      } else {
        rightIsCover = true;
        rightIsBackCover = true;
      }
    }
    
    // initialize rotation angle for changed direction
    if (oldLeftToRight != leftToRight)
      rotationTransform.angle = rotationOrigin;
    
    finalPageNumber = index;
    finalPageIsCover = false;
    
    return true;
  }
  
  // internal
  function start(automatic) {
    if (isRunning) {
      console.log("Blocking start of page flip animation");
      
      return;
    }
    
    // reset
    pageFlipped = false;
    
    clipper.visible = true;
    
    if (automatic) {
      if (rotationTransform.angle == rotationDestination)
        flippable.rotationCompleted();
      else
        clipper.triggerAutomatic = true;
    } else if (endFraction <= 0.0) {
      rotationTransform.angle = rotationOrigin;
    } else if (endFraction >= 1.0) {
      rotationTransform.angle = rotationDestination;
    } else {
      // 0.0 < endFraction < 1.0; endRotation calculates the appropriate angle
      rotationTransform.angle = endRotation;
    }
  }
  
  onAlbumChanged: setToAlbumCurrent()

  onVisibleChanged: setToAlbumCurrent()
  
  Item {
    // This clipping item exists to prevent half of the page from showing up
    // (the half we aren't animating yet).  It's arbitrarily bigger than the
    // parent region (so we don't clip the borders of the children) and
    // positioned so its edge is right down the middle of the page, hence the
    // odd negative absolute coordinates here.
    x: -(parent.width / 2)
    y: -(parent.height / 2)
    width: parent.width
    height: parent.height * 2
    
    Rectangle {
      x: albumPageViewer.width / 2
      y: albumPageViewer.height / 2
      width: albumPageViewer.width / 2
      height: albumPageViewer.height
      
      color: "transparent"
      
      AlbumCover {
        id: leftCoverBackground
        
        album: albumPageViewer.album
        
        width: albumPageViewer.width
        height: albumPageViewer.height
        
        visible: leftIsCover

        titleOpacity: albumPageViewer.coverTitleOpacity
      }
      
      AlbumPageComponent {
        id: leftPageBackground
        
        albumPage: leftPage
        
        width: albumPageViewer.width
        height: albumPageViewer.height
        
        topGutter: albumPageViewer.topGutter
        bottomGutter: albumPageViewer.bottomGutter
        leftGutter: albumPageViewer.leftGutter
        rightGutter: albumPageViewer.rightGutter
        spineGutter: albumPageViewer.spineGutter
        insideGutter: albumPageViewer.insideGutter

        visible: !leftIsCover

        isPreview: albumPageViewer.isPreview
      }
    }
  }
  
  Item {
    // See the note in the previous item about these odd absolute coordinates.
    x: parent.width / 2
    y: -(parent.height / 2)
    width: parent.width
    height: parent.height * 2
    
    clip: true
    
    Rectangle {
      x: -(albumPageViewer.width / 2)
      y: albumPageViewer.height / 2
      width: albumPageViewer.width / 2
      height: albumPageViewer.height
      
      color: "transparent"
      
      AlbumCover {
        id: rightCoverBackground
        
        album: albumPageViewer.album
        anchorRight: !rightIsBackCover
        isBlank: rightIsBackCover
        
        width: albumPageViewer.width
        height: albumPageViewer.height
        
        visible: rightIsCover

        titleOpacity: albumPageViewer.coverTitleOpacity
      }
      
      AlbumPageComponent {
        id: rightPageBackground
        
        albumPage: rightPage
        
        width: albumPageViewer.width
        height: albumPageViewer.height
        
        topGutter: albumPageViewer.topGutter
        bottomGutter: albumPageViewer.bottomGutter
        leftGutter: albumPageViewer.leftGutter
        rightGutter: albumPageViewer.rightGutter
        spineGutter: albumPageViewer.spineGutter
        insideGutter: albumPageViewer.insideGutter

        visible: !rightIsCover

        isPreview: albumPageViewer.isPreview
      }
    }
  }
  
  Item {
    id: clipper
    
    property bool triggerAutomatic: false
    
    // Similar to the above two items, this clipping item exists to prevent
    // half of the flippable from being seen.  See the note about the above
    // items' odd absolute positioning.
    x: leftToRight ? (parent.width / 2) : -(parent.width / 2)
    y: -(parent.height / 2)
    width: parent.width
    height: parent.height * 2
    
    clip: true
    
    Flipable {
      id: flippable
      
      // internal
      // Called when a page has completely flipped (rotation is completed)
      function rotationCompleted() {
        var pageFlipped = false;
        if (finalPageIsCover) {
          if (!album.closed) {
            pageFlipped = true;
            album.closed = true;
          }
        } else {
          if (album.currentPageNumber != finalPageNumber) {
            pageFlipped = true;
            album.currentPageNumber = finalPageNumber;
          }
          if (album.closed) {
            pageFlipped = true;
            album.closed = false;
          }
        }
        
        // set up this component to display the final page by hiding
        // the sliding clipping region and configuring the left and
        // right pages
        if (album.closed) {
          leftIsCover = true;
          rightIsCover = true;
          rightIsBackCover = false;
        } else {
          leftIsCover = false;
          rightIsCover = false;
          
          leftPage = album.currentPage;
          rightPage = album.currentPage;
        }
        
        // disable trigger
        clipper.triggerAutomatic = false;
        
        // hide the clipper so the background components can be seen
        clipper.visible = false;
        
        // reset rotation transformation for the next pass
        rotationTransform.angle = rotationOrigin;
        
        // notify subscribers
        if (pageFlipped)
          albumPageViewer.pageFlipped = true;
        else
          pageReleased();
      }
      
      x: leftToRight ? -(parent.width / 2) : (parent.width / 2)
      y: albumPageViewer.height / 2
      width: albumPageViewer.width
      height: albumPageViewer.height
      
      front: Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        
        AlbumCover {
          id: leftCoverAnimated
          
          album: albumPageViewer.album
          
          width: albumPageViewer.width
          height: albumPageViewer.height
          
          visible: leftIsCover

          titleOpacity: albumPageViewer.coverTitleOpacity
        }
        
        AlbumPageComponent {
          id: leftPageAnimated
          
          albumPage: leftPage
          
          width: albumPageViewer.width
          height: albumPageViewer.height
          
          topGutter: albumPageViewer.topGutter
          bottomGutter: albumPageViewer.bottomGutter
          leftGutter: albumPageViewer.leftGutter
          rightGutter: albumPageViewer.rightGutter
          spineGutter: albumPageViewer.spineGutter
          insideGutter: albumPageViewer.insideGutter

          visible: !leftIsCover

          isPreview: albumPageViewer.isPreview
        }
      }
      
      back: Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        
        AlbumCover {
          id: rightCoverAnimated
          
          album: albumPageViewer.album
          anchorRight: false
          isBlank: true
          
          width: albumPageViewer.width
          height: albumPageViewer.height
          
          visible: rightIsCover

          titleOpacity: albumPageViewer.coverTitleOpacity
        }
        
        AlbumPageComponent {
          id: rightPageAnimated
          
          albumPage: rightPage
          
          width: albumPageViewer.width
          height: albumPageViewer.height
          
          topGutter: albumPageViewer.topGutter
          bottomGutter: albumPageViewer.bottomGutter
          leftGutter: albumPageViewer.leftGutter
          rightGutter: albumPageViewer.rightGutter
          spineGutter: albumPageViewer.spineGutter
          insideGutter: albumPageViewer.insideGutter

          visible: !rightIsCover

          isPreview: albumPageViewer.isPreview
        }
      }
        
      transform: Rotation {
        id: rotationTransform
        
        origin.x: albumPageViewer.width / 2
        origin.y: albumPageViewer.height / 2
        
        axis.x: 0
        axis.y: 1
        axis.z: 0
        
        angle: startRotation
        
        onAngleChanged: {
          // slide the clipping region left or right depending on the angle
          // of rotation of the flippable
          if (angle <= -90.0) {
            clipper.x = -albumPageViewer.width / 2;
            flippable.x = albumPageViewer.width / 2;
          } else {
            clipper.x = albumPageViewer.width / 2;
            flippable.x = -albumPageViewer.width / 2;
          }
        }
      }
      
      states: State {
        name: "animateAutomatic"
        
        PropertyChanges {
          target: rotationTransform
          angle: endRotation
        }
        
        when: clipper.triggerAutomatic
      }
      
      transitions: Transition {
        to: "animateAutomatic"
        reversible: false
        
        SequentialAnimation {
          NumberAnimation {
            target: rotationTransform
            property: "angle"
            duration: durationMsec
            easing.type: Easing.InOutCubic
          }
          
          ScriptAction {
            script: {
              if (rotationTransform.angle == rotationDestination || rotationTransform.angle == rotationOrigin)
                flippable.rotationCompleted();
              else
                clipper.triggerAutomatic = false;
            }
          }
        }
      }
    }
  }
}

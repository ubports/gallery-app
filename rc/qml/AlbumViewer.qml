/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  id: albumViewer
  objectName: "albumViewer"
  
  property Album album
  property int borderWidth: gu(0.25)
  property alias pageTop: albumPageViewer.y
  property alias pageHeight: albumPageViewer.height
  
  // When the user clicks the back button.
  signal closeRequested()

  anchors.fill: parent

  onAlbumChanged: {
    if (album)
      albumPageViewer.setTo(album.currentPageNumber);
  }
  
  AlbumPageViewer {
    id: albumPageViewer
    
    anchors.top: masthead.bottom
    anchors.bottom: pageIndicator.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    album: albumViewer.album
    
    SwipeArea {
      property real commitTurnFraction: 0.35
      
      // private
      property int turningTowardPage
      
      anchors.fill: parent
      
      enabled: !parent.isRunning
      
      onStartSwipe: {
        turningTowardPage = album.currentPageNumber + (leftToRight ? -1 : 1);
      }
      
      onSwiping: {
        var availableDistance = (leftToRight) ? (width - start) : start;
        albumPageViewer.turnToward(turningTowardPage, distance / availableDistance);
      }
      
      onSwiped: {
        if (albumPageViewer.currentFraction >= commitTurnFraction)
          albumPageViewer.turnTo(turningTowardPage);
        else
          albumPageViewer.releasePage();
      }
    }
  }
  
  AlbumPageIndicator {
    id: pageIndicator
    
    y: parent.height - chrome.toolbarHeight - height
    width: parent.width
    height: gu(3)
    
    color: "transparent"
    
    album: albumViewer.album
    
    onSelected: {
      albumPageViewer.turnTo(pageNumber);
    }
  }
  
  Rectangle {
    id: middleBorder

    width: borderWidth
    height: parent.height
    anchors.centerIn: parent
    color: "#95b5de"
  }

  Rectangle {
    id: masthead

    width: parent.width
    height: gu(6)

    Text {
      anchors.centerIn: parent

      text: (album) ? album.name : ""

      color: "#657CA9"
    }
  }

  ViewerChrome {
    id: chrome

    z: 10
    anchors.fill: parent

    state: "shown"
    visible: true

    leftNavigationButtonVisible: false
    rightNavigationButtonVisible: false

    toolbarHasFullIconSet: false

    popupMenuItemTitle: "Add photos"

    autoHideWait: 0

    onMenuItemChosen: navStack.switchToMediaSelector(album)

    onReturnButtonPressed: closeRequested()
  }
}

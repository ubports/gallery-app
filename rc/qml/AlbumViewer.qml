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
  property alias pageTop: albumPageViewer.y
  property alias pageHeight: albumPageViewer.height
  
  // When the user clicks the back button.
  signal closeRequested()

  anchors.fill: parent

  state: "pageView"

  states: [
    State { name: "pageView"; },
    State { name: "gridView"; }
  ]

  transitions: [
    Transition { from: "pageView"; to: "gridView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: albumPageViewer; fadeInTarget: gridCheckerboard; }
      }
    },
    Transition { from: "gridView"; to: "pageView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: gridCheckerboard; fadeInTarget: albumPageViewer; }
      }
    }
  ]
  
  function resetView() {
    state = ""; // Prevents the animation on gridView -> pageView from happening.
    state = "pageView";
    albumPageViewer.visible = true;
    gridCheckerboard.visible = false;
  }

  onAlbumChanged: {
    if (album)
      albumPageViewer.setTo(album.currentPageNumber);
  }
  
  AlbumPageViewer {
    id: albumPageViewer
    
    anchors.fill: parent
    anchors.topMargin: chrome.navbarHeight
    anchors.bottomMargin: chrome.toolbarHeight
    
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
  
  Checkerboard {
    id: gridCheckerboard
    objectName: "gridCheckerboard"
    
    anchors.fill: parent
    anchors.topMargin: chrome.navbarHeight + gu(3)
    anchors.leftMargin: gu(2.75)
    anchors.rightMargin: gu(2.75)
    
    visible: false
    
    allowSelection: true
    
    model: MediaCollectionModel {
      forCollection: album
    }
    
    delegate: PhotoComponent {
      anchors.centerIn: parent
      
      width: parent.width
      height: parent.height
      
      mediaSource: modelData.mediaSource
      isCropped: true
      isPreview: true
      ownerName: "AlbumViewer grid"
    }

    onActivated: photoViewer.animateOpen(object, activatedRect)
  }

  AlbumViewerOptionsMenu {
    id: albumViewerOptionsMenu

    popupOriginX: parent.width - gu(5);

    visible: false;
  }

  ViewerChrome {
    id: chrome

    z: 10
    anchors.fill: parent

    navbarTitle: (album) ? album.name : ""

    state: "shown"
    visible: true

    autoHideWait: 0

    inSelectionMode: gridCheckerboard.visible && gridCheckerboard.inSelectionMode

    toolbarsAreTranslucent: (albumViewer.state == "gridView")

    navbarHasStateButton: true
    navbarStateButtonIconFilename: (albumViewer.state == "pageView") ? "../img/grid-view.png" :
      "../img/template-view.png";

    toolbarHasFullIconSet: false
    toolbarHasPageIndicator: albumViewer.state == "pageView"
    toolbarPageIndicatorAlbum: albumViewer.album

    onPageIndicatorPageSelected: albumPageViewer.turnTo(pageNumber)

    onStateButtonPressed: {
      albumViewer.state = (albumViewer.state == "pageView" ? "gridView" : "pageView");
    }

    onSelectionDoneButtonPressed: {
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
    }

    onNewAlbumPicked: {
      gridCheckerboard.model.createAlbumFromSelected();
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
    }

    onAlbumPicked: {
      album.addSelectedMediaSources(gridCheckerboard.model);
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
    }

    onReturnButtonPressed: {
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();

      closeRequested();
    }

    onMoreOperationsButtonPressed: {
      albumViewerOptionsMenu.flipVisibility();
    }
  }

  PopupPhotoViewer {
    id: photoViewer

    anchors.fill: parent
    z: 100

    onOpening: {
      model = gridCheckerboard.model
    }
    
    onIndexChanged: {
      gridCheckerboard.ensureIndexVisible(index, false);
    }

    onCloseRequested: {
      var thumbnailRect = gridCheckerboard.getRectOfItemAt(index, photoViewer);
      if (thumbnailRect)
        animateClosed(thumbnailRect);
      else
        close();
    }
  }
}

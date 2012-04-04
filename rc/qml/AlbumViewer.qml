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
import "../Capetown"
import "GalleryUtility.js" as GalleryUtility

Rectangle {
  id: albumViewer
  objectName: "albumViewer"
  
  property Album album
  
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
        DissolveAnimation { fadeOutTarget: albumSpreadViewer; fadeInTarget: gridCheckerboard; }
      }
    },
    Transition { from: "gridView"; to: "pageView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: gridCheckerboard; fadeInTarget: albumSpreadViewer; }
      }
    }
  ]
  
  function resetView(album) {
    albumViewer.album = album;

    state = ""; // Prevents the animation on gridView -> pageView from happening.
    state = "pageView";

    albumSpreadViewer.visible = true;
    chrome.show();
    gridCheckerboard.visible = false;
  }

  AlbumSpreadViewer {
    id: albumSpreadViewer
    
    anchors.fill: parent
    
    album: albumViewer.album
    
    onPageFlipped: {
      // turn chrome back on once flip is completed
      chrome.show();
    }
    
    onPageReleased: {
      chrome.show();
    }
    
    SwipeArea {
      property real commitTurnFraction: 0.05
      
      // private
      property int turningTowardPage
      
      anchors.fill: parent
      
      enabled: !parent.isRunning
      
      onStartSwipe: {
        turningTowardPage = album.currentPage + (leftToRight ? -2 : 2); // 2 pages per spread.
        albumSpreadViewer.turnTowardPage = turningTowardPage;
        
        // turn off chrome, allow the page flipper full screen
        chrome.hide();
      }
      
      onSwiping: {
        var availableDistance = (leftToRight) ? (width - start) : start;
        albumSpreadViewer.turnFraction = (distance / availableDistance);
      }
      
      onSwiped: {
        // Can turn toward the cover, but never close the album in the viewer
        if (albumSpreadViewer.currentFraction >= commitTurnFraction
          && turningTowardPage > album.firstValidCurrentPage
          && turningTowardPage < album.lastValidCurrentPage)
          albumSpreadViewer.turnTo(turningTowardPage);
        else
          albumSpreadViewer.releasePage();
      }
    }
  }
  
  Checkerboard {
    id: gridCheckerboard
    objectName: "gridCheckerboard"
    
    anchors.fill: parent
    anchors.topMargin: chrome.navbarHeight + gu(2)
    anchors.leftMargin: gu(2)
    anchors.rightMargin: gu(2)
    
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

    onActivated: {
      var photoRect = GalleryUtility.translateRect(activatedRect, gridCheckerboard, photoViewer);
      photoViewer.animateOpen(object, photoRect);
    }
  }

  ViewerChrome {
    id: chrome

    z: 10
    anchors.fill: parent

    navbarTitle: (album) ? album.name : ""

    state: "shown"
    visible: true

    fadeDuration: 0
    autoHideWait: 0

    inSelectionMode: gridCheckerboard.visible && gridCheckerboard.inSelectionMode

    toolbarsAreTranslucent: (albumViewer.state == "gridView")

    navbarHasStateButton: true
    navbarStateButtonIconFilename: (albumViewer.state == "pageView") ? "../img/grid-view.png" :
      "../img/template-view.png";

    toolbarHasFullIconSet: false
    toolbarHasPageIndicator: albumViewer.state == "pageView"
    toolbarPageIndicatorAlbum: albumViewer.album

    property variant popups: [ albumViewerOptionsMenu, albumViewerShareMenu ]

    AlbumViewerOptionsMenu {
      id: albumViewerOptionsMenu
      
      popupOriginX: -gu(1.5)
      popupOriginY: -gu(6)
      
      // a switch-case case statement instead of an if statement because we
      // soon hope to be able to respond to all six menu items
      onActionInvoked: {
        switch (name) {
          case "onAddPhotos":
            navStack.switchToMediaSelector(albumViewer.album);
          break;
        }
      }

      onPopupInteractionCompleted: {
        hideAllPopups();
      }

      visible: false;
    }

    AlbumViewerShareMenu {
      id: albumViewerShareMenu
      
      popupOriginX: -gu(9)
      popupOriginY: -gu(6)
      
      onPopupInteractionCompleted: {
        hideAllPopups();
      }

      visible: false
    }

    onPageIndicatorPageSelected: albumSpreadViewer.turnTo(page)

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
      cyclePopup(albumViewerOptionsMenu);
    }

    onShareOperationsButtonPressed: {
      cyclePopup(albumViewerShareMenu);
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

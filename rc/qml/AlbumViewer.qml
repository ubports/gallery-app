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
        FadeOutAnimation { target: pageIndicator; }
      }
    },
    Transition { from: "gridView"; to: "pageView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: gridCheckerboard; fadeInTarget: albumPageViewer; }
        FadeInAnimation { target: pageIndicator; }
      }
    }
  ]
  
  function resetView() {
    state = ""; // Prevents the animation on gridView -> pageView from happening.
    state = "pageView";
    albumPageViewer.visible = true;
    gridCheckerboard.visible = false;
    masthead.isTemplateView = true;
  }

  onAlbumChanged: {
    if (album)
      albumPageViewer.setTo(album.currentPageNumber);
  }
  
  AlbumViewMasthead {
    id: masthead
    objectName: "masthead"

    albumName: (album) ? album.name : ""

    areItemsSelected: gridCheckerboard.selectedCount > 0

    x: 0
    y: 0
    width: parent.width

    onViewModeChanged: {
      if (isTemplateView) {
        albumViewer.state = "pageView";
      } else {
        albumViewer.state = "gridView";
      }
    }

    onSelectionInteractionCompleted: {
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
    }

    onDeselectAllRequested: {
      gridCheckerboard.unselectAll();
    }
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
  
  Checkerboard {
    id: gridCheckerboard
    objectName: "gridCheckerboard"
    
    anchors.top: masthead.bottom
    height: parent.height - masthead.height - chrome.toolbarHeight
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.topMargin: gu(3)
    anchors.bottomMargin: 0
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

    onInSelectionModeChanged: {
      masthead.isSelectionInProgress = inSelectionMode
    }
    
    onActivated: photoViewer.animateOpen(object, activatedRect)
  }

  AlbumPageIndicator {
    id: pageIndicator
    
    y: parent.height - chrome.toolbarHeight - height
    width: parent.width
    height: gu(3)
    
    color: "transparent"
    visible: (album) ? album.pageCount > 1 : false;
    
    album: albumViewer.album
    
    onSelected: {
      albumPageViewer.turnTo(pageNumber);
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

    toolbarHasFullIconSet: (gridCheckerboard.visible && gridCheckerboard.selectedCount > 0)
    toolbarUseContrastOnWhiteColorScheme: gridCheckerboard.visible

    popupMenuItemTitle: "Add photos"

    autoHideWait: 0

    onMenuItemChosen: navStack.switchToMediaSelector(album)

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
      masthead.isSelectionInProgress = false
      masthead.areItemsSelected = false
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();

      closeRequested();
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
      gridCheckerboard.ensureIndexVisible(index);
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

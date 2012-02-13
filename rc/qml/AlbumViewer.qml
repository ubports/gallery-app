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
  property int borderWidth: 2
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
        FadeOutAnimation { target: middleBorder; }
        FadeOutAnimation { target: pageIndicator; }
      }
    },
    Transition { from: "gridView"; to: "pageView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: gridCheckerboard; fadeInTarget: albumPageViewer; }
        FadeInAnimation { target: middleBorder; }
        FadeInAnimation { target: pageIndicator; }
      }
    }
  ]
  
  function resetView() {
    state = ""; // Prevents the animation on gridView -> pageView from happening.
    state = "pageView";
    albumPageViewer.visible = true;
    gridCheckerboard.visible = false;
    middleBorder.visible = true;
    masthead.isTemplateView = true;
    pageIndicator.visible = true;
  }

  onAlbumChanged: {
    if (album)
      albumPageViewer.setTo(album.currentPageNumber);
  }
  
  PlaceholderPopupMenu {
    id: addPhotosMenu

    z: 20
    anchors.bottom: toolbar.top
    x: toolbar.moreOperationsPopupX - width;

    itemTitle: "Add photos"

    state: "hidden"

    onItemChosen: {
      navStack.switchToMediaSelector(album)
    }
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
      anchors.fill: parent
      
      active: !parent.isRunning
      
      onSwiped: {
        albumPageViewer.turnTo(album.currentPageNumber + (leftToRight ? 1 : -1));
      }
    }
  }
  
  Checkerboard {
    id: gridCheckerboard
    objectName: "gridCheckerboard"
    
    anchors.top: masthead.bottom
    anchors.bottom: toolbar.top
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
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

    MouseArea {
      id: mouse_blocker
      objectName: "mouse_blocker"

      anchors.fill: parent

      onClicked: {
        if (addPhotosMenu.state == "shown" || album_picker.state == "shown") {
          addPhotosMenu.state = "hidden"
          album_picker.state = "hidden"
          return;
        }
      }

      visible: (addPhotosMenu.state == "shown" || album_picker.state == "shown")
    }

    onInSelectionModeChanged: {
      masthead.isSelectionInProgress = inSelectionMode
    }
    
    onActivated: photoViewer.animateOpen(object, activatedRect)
  }

  AlbumPickerPopup {
    id: album_picker
    objectName: "album_picker"

    y: parent.height - height - toolbar.height
    x: toolbar.albumOperationsPopupX - width

    designated_model: AlbumCollectionModel {
    }

    state: "hidden"

    onNewAlbumRequested: {
      gridCheckerboard.model.createAlbumFromSelected();
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
      state = "hidden"
    }

    onSelected: {
      album.addSelectedMediaSources(gridCheckerboard.model);
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();
    }
  }
  
  AlbumPageIndicator {
    id: pageIndicator
    
    anchors.bottom: toolbar.top
    
    width: parent.width
    height: 24
    
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

  GalleryStandardToolbar {
    id: toolbar
    objectName: "toolbar"

    hasFullIconSet: (gridCheckerboard.visible && gridCheckerboard.selectedCount > 0)
    useContrastOnWhiteColorScheme: gridCheckerboard.visible

    z: 10
    anchors.bottom: parent.bottom

    onReturnButtonPressed: {
      album_picker.visible = false
      masthead.isSelectionInProgress = false
      masthead.areItemsSelected = false
      gridCheckerboard.state = "normal";
      gridCheckerboard.unselectAll();

      closeRequested();
    }

    onMoreOperationsButtonPressed: addPhotosMenu.flipVisibility();

    onAlbumOperationsButtonPressed: album_picker.flipVisibility();
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

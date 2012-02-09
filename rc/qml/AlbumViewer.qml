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
  property alias pageTop: templatePager.y
  property alias pageHeight: templatePager.height
  
  // When the user clicks the back button.
  signal closeRequested()

  anchors.fill: parent

  state: "pageView"

  states: [
    State { name: "pageView"; },
    State { name: "gridView"; },
    State { name: "albumCover"; }
  ]

  transitions: [
    Transition { from: "albumCover"; to: "gridView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: albumCover; fadeInTarget: gridCheckerboard; }
        FadeOutAnimation { target: middleBorder; }
        FadeOutAnimation { target: pageIndexerPlaceholder; }
      }
    },
    Transition { from: "pageView"; to: "gridView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: templatePager; fadeInTarget: gridCheckerboard; }
        FadeOutAnimation { target: middleBorder; }
        FadeOutAnimation { target: pageIndexerPlaceholder; }
      }
    },
    Transition { from: "gridView"; to: "albumCover";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: gridCheckerboard; fadeInTarget: albumCover; }
        FadeInAnimation { target: middleBorder; }
        FadeInAnimation { target: pageIndexerPlaceholder; }
      }
    },
    Transition { from: "gridView"; to: "pageView";
      ParallelAnimation {
        DissolveAnimation { fadeOutTarget: gridCheckerboard; fadeInTarget: templatePager; }
        FadeInAnimation { target: middleBorder; }
        FadeInAnimation { target: pageIndexerPlaceholder; }
      }
    }
  ]
  
  function resetView() {
    state = ""; // Prevents the animation on gridView -> pageView from happening.
    state = (album.isClosed) ? "albumCover" : "pageView";
    gridCheckerboard.visible = false;
    middleBorder.visible = true;
    masthead.isTemplateView = true;
  }

  onAlbumChanged: {
    if (album)
      templatePager.pageTo(album.currentPageNumber);
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

    albumName: templatePager.albumName

    areItemsSelected: gridCheckerboard.selectedCount > 0

    x: 0
    y: 0
    width: parent.width

    onViewModeChanged: {
      if (isTemplateView) {
        albumViewer.state = (album.isClosed) ? "albumCover" : "pageView";
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
  
  Pager {
    id: templatePager
    objectName: "templatePager"
    
    property string albumName

    anchors.fill: undefined
    anchors.top: masthead.bottom
    anchors.bottom: pageIndexerPlaceholder.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    pageCacheSize: 1
    
    visible: (album) ? (!album.isClosed && !pageFlipAnimation.visible) : false
    
    model: AlbumPageModel {
      forAlbum: album
    }
    
    delegate: AlbumPageComponent {
      albumPage: object
      
      width: templatePager.width
      height: templatePager.height
    }
    
    interactive: false
    
    onCurrentIndexChanged: {
      // Can't open an album by changing the currentIndex; this is esp.
      // problematic at initialization, when the currentIndex is set internally
      // but not by user interaction
      if (album && !album.isClosed)
        album.currentPageNumber = currentIndex;
    }
    
    SwipeArea {
      anchors.fill: parent
      
      onSwiped: {
        if (pageFlipAnimation.isRunning)
          return;
        
        var curr = null;
        var next = null;
        
        var ofs = (leftToRight) ? 1 : -1;
        if ((parent.currentIndex + ofs) < 0) {
          album.close();
          pageFlipAnimation.leftIsCover = true;
        } else {
          next = parent.model.getAt(parent.currentIndex + ofs);
          pageFlipAnimation.leftIsCover = false;
        }
        
        curr = parent.model.getAt(parent.currentIndex);
        
        if ((!pageFlipAnimation.leftIsCover) && (curr == null || next == null))
          return;
        
        pageFlipAnimation.leftToRight = leftToRight;
        pageFlipAnimation.leftPage = (leftToRight) ? curr : next;
        pageFlipAnimation.rightPage = (leftToRight) ? next : curr;
        
        pageFlipAnimation.start();
        
        if (leftToRight)
          parent.pageForward();
        else if (album && !album.isClosed)
          parent.pageBack();
      }
      
      onReleased: {
        startX = -1;
      }
    }
  }
  
  AlbumCover {
    id: albumCover
    
    album: albumViewer.album
    
    anchors.fill: templatePager
    
    visible: (album) ? (album.isClosed && !pageFlipAnimation.visible) : false
    
    SwipeArea {
      anchors.fill: parent
      
      onSwiped: {
        if (!leftToRight || pageFlipAnimation.isRunning)
          return;
        
        pageFlipAnimation.leftToRight = true;
        pageFlipAnimation.leftIsCover = true;
        pageFlipAnimation.rightPage = templatePager.model.getAt(0);
        
        album.currentPageNumber = 0;
        
        pageFlipAnimation.start();
      }
    }
  }
  
  AlbumPageFlipAnimation {
    id: pageFlipAnimation
    
    anchors.fill: templatePager
    
    cover: albumViewer.album
    
    visible: isRunning
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

  Rectangle {
    id: pageIndexerPlaceholder

    width: parent.width
    height: 24
    anchors.bottom: toolbar.top
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

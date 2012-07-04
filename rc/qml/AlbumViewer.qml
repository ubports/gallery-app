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

  property Album album

  // When the user clicks the back button or pages back to the cover.
  signal closeRequested(bool stayOpen, bool backCoverCloseAnimation)
  signal editPhotoRequested(variant photo)

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
  
  onStateChanged: {
    if (state == "pageView") {
      gridCheckerboard.unselectAll();
      gridCheckerboard.inSelectionMode = false;
    }
  }
  
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
    
    // Keyboard focus while visible
    focus: visible == true

    onPageFlipped: chrome.show()
    onPageReleased: chrome.show()
    
    Keys.onPressed: {
      if (event.key === Qt.Key_Left &&
        album.currentPage > album.firstContentPage &&
        !albumSpreadViewer.isFlipping) {
        
        chrome.hide();
        albumSpreadViewer.flipTo(album.currentPage -2 ); // 2 pages per spread
        event.accepted = true;
      } else if (event.key === Qt.Key_Right &&
        album.currentPage < albumSpreadViewer.getLeftHandPageNumber(album.lastContentPage) &&
        !albumSpreadViewer.isFlipping) {
        
        chrome.hide();
        albumSpreadViewer.flipTo(album.currentPage + 2); // 2 pages per spread
        event.accepted = true;
      }
    }
    
    SwipeArea {
      property real commitTurnFraction: 0.05
      property real backCoverCloseFraction: 0.25

      anchors.fill: parent

      enabled: !parent.isRunning
      
      onTapped: {
        var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
        if (!hit || !hit.mediaSource)
          return;
        
        if (gridCheckerboard.inSelectionMode) {
          gridCheckerboard.model.toggleSelection(hit.mediaSource);
        } else {
          var rect = GalleryUtility.getRectRelativeTo(hit, photoViewer);
          photoViewer.forGridView = false;
          photoViewer.animateOpen(hit.mediaSource, rect, false);
        }
      }

      onLongPressed: {
        var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
        if (!hit || !hit.mediaSource)
          return;
        
        albumPagePhotoMenu.positionRelativeTo(hit.mediaSource);
        chrome.cyclePopup(albumPagePhotoMenu);
      }
      
      onStartSwipe: {
        albumSpreadViewer.destinationPage =
            album.currentPage + (leftToRight ? -2 : 2); // 2 pages per spread.

        // turn off chrome, allow the page flipper full screen
        chrome.hide();
      }

      onSwiping: {
        if (leftToRight && album.currentPage == album.firstContentPage) {
          closeRequested(false, false);
          return;
        }

        var availableDistance = (leftToRight) ? (width - start) : start;
        // TODO: the 0.999 here is kind of a hack.  The AlbumPageFlipper
        // can't tell the difference between its flipFraction being set to 1
        // from the drag vs. its own animation.  So I don't let the drag set it
        // quite all the way to 1.  I should somehow fix this shortcoming in
        // the AlbumPageFlipper, but this is fine for now.
        var flipFraction =
            Math.max(0, Math.min(0.999, distance / availableDistance));
        albumSpreadViewer.flipFraction = flipFraction;

        if (!leftToRight &&
            album.currentPage == albumSpreadViewer.getLeftHandPageNumber(album.lastContentPage) &&
            flipFraction >= backCoverCloseFraction) {
          closeRequested(false, true);
          albumSpreadViewer.flipFraction = 0;
          return;
        }
      }

      onSwiped: {
        // Can turn toward the cover, but never close the album in the viewer
        if (albumSpreadViewer.flipFraction >= commitTurnFraction &&
            albumSpreadViewer.destinationPage > album.firstValidCurrentPage &&
            albumSpreadViewer.destinationPage < album.lastValidCurrentPage)
          albumSpreadViewer.flip();
        else
          albumSpreadViewer.release();
      }
    }
  }

  Checkerboard {
    id: gridCheckerboard
    
    anchors.fill: parent
    header: Item {
      height: chrome.navbarHeight
      width: 1
    }

    topExtraGutter: gu(2)
    bottomExtraGutter: gu(0)
    leftExtraGutter: gu(2)
    rightExtraGutter: gu(2)

    visible: false
    
    allowSelectionModeChange: true
    
    model: MediaCollectionModel {
      forCollection: album
    }

    delegate: CheckerboardDelegate {
      id: gridCheckerboardDelegate
      
      checkerboard: gridCheckerboard
      
      content: MattedPhotoPreview {
        ownerName: "AlbumViewer grid"
        mediaSource: modelData.mediaSource
        isSelected: gridCheckerboardDelegate.isSelected
      }
    }

    onActivated: {
      var photoRect = GalleryUtility.translateRect(activatedRect, gridCheckerboard, photoViewer);
      photoViewer.forGridView = true;
      photoViewer.animateOpen(object, photoRect, true);
    }
  }

  ViewerChrome {
    id: chrome

    anchors.fill: parent

    state: "shown"
    visible: true

    fadeDuration: 0
    autoHideWait: 0

    inSelectionMode: gridCheckerboard.inSelectionMode
    hasSelectionNavbar: false

    hasSelectionOperationsButton: inSelectionMode
    onSelectionOperationsButtonPressed: cyclePopup(selectionMenu)

    toolbarsAreTranslucent: (albumViewer.state == "gridView")
    toolbarsAreTextured: (albumViewer.state == "gridView")

    navbarHasStateButton: true
    navbarSelectedStateButtonIconFilename: (albumViewer.state == "pageView"
      ? "../img/icon-grid-view-active.png"
      : "../img/icon-album-view-active.png")
    navbarDeselectedStateButtonIconFilename: (albumViewer.state == "pageView"
      ? "../img/icon-grid-view-inactive.png"
      : "../img/icon-album-view-inactive.png")

    toolbarHasFullIconSet: false
    toolbarHasAlbumOperationsButton: false
    toolbarHasPageIndicator: albumViewer.state == "pageView"
    toolbarPageIndicatorAlbum: albumViewer.album

    popups: [ albumViewerOptionsMenu, albumViewerShareMenu,
      selectionMenu, trashDialog, albumPagePhotoMenu, trashFromAlbumPageDialog ]

    onPageIndicatorPageSelected: {
      chrome.hide();
      albumSpreadViewer.flipTo(page);
    }

    onStateButtonPressed: {
      albumViewer.state = (albumViewer.state == "pageView" ? "gridView" : "pageView");
    }

    onSelectionDoneButtonPressed: {
      gridCheckerboard.unselectAll();
      gridCheckerboard.inSelectionMode = false;
    }

    onReturnButtonPressed: {
      gridCheckerboard.unselectAll();
      gridCheckerboard.inSelectionMode = false;

      closeRequested(true, false);
    }

    onMoreOperationsButtonPressed: cyclePopup(albumViewerOptionsMenu)
    onShareOperationsButtonPressed: cyclePopup(albumViewerShareMenu)
    onTrashOperationButtonPressed: cyclePopup(trashDialog)

    SelectionMenu {
      id: selectionMenu

      checkerboard: gridCheckerboard

      onPopupInteractionCompleted: chrome.hideAllPopups()
    }

    AlbumViewerOptionsMenu {
      id: albumViewerOptionsMenu

      popupOriginX: -gu(1.5)
      popupOriginY: -gu(6)

      // a switch-case case statement instead of an if statement because we
      // soon hope to be able to respond to all six menu items
      onActionInvoked: {
        switch (name) {
          case "onAddPhotos":
            mediaSelector.show();
          break;
        }
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()

      visible: false;
    }

    AlbumViewerShareMenu {
      id: albumViewerShareMenu

      popupOriginX: -gu(9)
      popupOriginY: -gu(6)

      onPopupInteractionCompleted: chrome.hideAllPopups()

      visible: false
    }
    
    // When delete is invoked from grid view
    DeleteRemoveDialog {
      id: trashDialog

      // internal
      function finishRemove() {
        gridCheckerboard.unselectAll();
        gridCheckerboard.inSelectionMode = false;

        // If all the photos were removed from the album and it was deleted,
        // album will now be set to null.
        if (!album) {
          albumViewer.closeRequested(true, false);
          return;
        }

        // In the Album model, the last valid current page is the back cover.
        // However, in the UI, we want to stay on the content pages.  The -1 is
        // since the last content page will be on the right, but current page
        // must always be the left page.
        if (album.currentPage > album.lastContentPage - 1)
          album.currentPage = album.lastContentPage - 1;
      }

      popupOriginX: -gu(16.5)
      popupOriginY: -gu(6)

      visible: false

      onRemoveRequested: {
        album.removeSelectedMediaSources(gridCheckerboard.model);

        finishRemove();
      }

      onDeleteRequested: {
        gridCheckerboard.model.destroySelectedMedia();

        finishRemove();
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()

      AlbumCollectionModel {
        id: trashModel
      }
    }
    
    AlbumPagePhotoMenu {
      id: albumPagePhotoMenu
      
      visible: false
      state: "hidden"
      
      property MediaSource mediaSource
      
      function positionRelativeTo(m) {
        mediaSource = m;
        var rect = albumSpreadViewer.getRectOfMediaSource(mediaSource);
        rect = GalleryUtility.getRectRelativeTo(rect, photoViewer);
        if (rect.x <= overview.width / 2)
          popupOriginX = rect.x + rect.width + gu(4);
        else
          popupOriginX = rect.x - childrenRect.width;
        
        popupOriginY = rect.y;
      }
      
      onActionInvoked: {
        // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you
        // edit a switch statement in QML.  The short version is: use braces
        // always.
        switch (name) {
          case "onExport": {
            // TODO
            break;
          }
          
          case "onPrint": {
            // TODO
            break;
          }
          
          case "onShare": {
            // TODO
            break;
          }
          
          case "onDelete": {
            trashFromAlbumPageDialog.popupOriginX = popupOriginX;
            trashFromAlbumPageDialog.popupOriginY = popupOriginY;
            trashFromAlbumPageDialog.media = mediaSource;
            chrome.cyclePopup(trashFromAlbumPageDialog);
            
            break;
          }
        }
      }
      
      onPopupInteractionCompleted: chrome.hideAllPopups()
    }
    
    // When delete is invoked from an album page
    DeleteRemoveDialog {
      id: trashFromAlbumPageDialog
      
      property MediaSource media
      
      visible: false
      
      onRemoveRequested: {
        album.removeMediaSource(media);
        
        trashDialog.finishRemove();
      }
      
      onDeleteRequested: {
        gridCheckerboard.model.destroyMedia(media);
        
        trashDialog.finishRemove();
      }
      
      onPopupInteractionCompleted: chrome.hideAllPopups()
      
      AlbumCollectionModel {
        id: trashFromAlbumPageModel
      }
    }
  }
  
  PopupPhotoViewer {
    id: photoViewer
    
    // true if the grid view component is using the photo viewer, false if the
    // album spread viewer is using it ... this should be set prior to 
    // opening the viewer
    property bool forGridView
    
    anchors.fill: parent

    onOpening: {
      // although this might be used by the page viewer, it too uses the grid's
      // models because you can walk the entire album from both
      model = gridCheckerboard.model;
    }

    onIndexChanged: {
      if (forGridView) {
        gridCheckerboard.ensureIndexVisible(index, false);
      } else {
        var page = album.getPageForMediaSource(photo);
        if (page >= 0)
          album.currentPage = albumSpreadViewer.getLeftHandPageNumber(page);
      }
    }

    onEditRequested: albumViewer.editPhotoRequested(photo)

    onCloseRequested: {
      // If all the photos were removed from the album and it was deleted,
      // album will now be set to null.
      if (!album) {
        close();
        albumViewer.closeRequested(true, false);
        return;
      }

      var rect = (forGridView)
        ? gridCheckerboard.getRectOfItemAt(index, photoViewer)
        : albumSpreadViewer.getRectOfMediaSource(photo);
      if (rect) {
        if (!forGridView)
          rect = GalleryUtility.getRectRelativeTo(rect, photoViewer);
        animateClosed(rect, false);
      } else {
        close();
      }
    }
  }
  
  MediaSelector {
    id: mediaSelector

    anchors.fill: parent

    album: albumViewer.album

    onCancelRequested: hide()

    onDoneRequested: {
      var firstPhoto = album.addSelectedMediaSources(model);

      hide();

      if (firstPhoto && albumViewer.state == "pageView") {
        var firstChangedPage = album.getPageForMediaSource(firstPhoto);
        var firstChangedSpread = albumSpreadViewer.getLeftHandPageNumber(firstChangedPage);

        chrome.hide();
        albumSpreadViewer.flipTo(firstChangedSpread);
      }
    }
  }
}

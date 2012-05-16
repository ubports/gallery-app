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
  signal closeRequested(bool stayOpen)
  signal addPhotosRequested(variant album)

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
    selectionCheckerboard: gridCheckerboard

    onPageFlipped: chrome.show()
    onPageReleased: chrome.show()

    SwipeArea {
      property real commitTurnFraction: 0.05

      // private
      property int turningTowardPage

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

        gridCheckerboard.inSelectionMode = !gridCheckerboard.inSelectionMode;
        if (gridCheckerboard.inSelectionMode)
          gridCheckerboard.model.toggleSelection(hit.mediaSource);
        else
          gridCheckerboard.unselectAll();
      }
      
      onStartSwipe: {
        turningTowardPage = album.currentPage + (leftToRight ? -2 : 2); // 2 pages per spread.
        albumSpreadViewer.turnTowardPage = turningTowardPage;

        // turn off chrome, allow the page flipper full screen
        chrome.hide();
      }

      onSwiping: {
        if (leftToRight && album.currentPage == album.firstContentPage) {
          closeRequested(false);
          return;
        }

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

  EventCheckerboard {
    id: gridCheckerboard
    objectName: "gridCheckerboard"
    
    property variant photoViewerModel: MediaCollectionModel {
      forCollection: albumViewer.album
      monitored: true
      monitorSelection: gridCheckerboard.model
    }
    
    anchors.fill: parent
    anchors.topMargin: chrome.navbarHeight

    topExtraGutter: gu(2)
    bottomExtraGutter: gu(0)
    leftExtraGutter: gu(2)
    rightExtraGutter: gu(2)

    visible: false
    
    forCollection: albumViewer.album
    allowSelectionModeChange: true
    ascending: true
    
    onActivated: {
      var photoRect = GalleryUtility.translateRect(activatedRect, gridCheckerboard, photoViewer);
      photoViewer.forGridView = true;
      photoViewer.animateOpen(object, photoRect, false);
    }
  }

  ViewerChrome {
    id: chrome

    z: 10
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

    toolbarIsTextured: (albumViewer.state == "gridView")
    navbarIsTextured: (albumViewer.state == "gridView")

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
      selectionMenu, trashDialog ]

    onPageIndicatorPageSelected: {
      chrome.hide();
      albumSpreadViewer.turnTo(page);
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

      closeRequested(true);
    }

    onMoreOperationsButtonPressed: cyclePopup(albumViewerOptionsMenu)
    onShareOperationsButtonPressed: cyclePopup(albumViewerShareMenu)
    onTrashOperationButtonPressed: cyclePopup(trashDialog)

    SelectionMenu {
      id: selectionMenu

      checkerboard: gridCheckerboard
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
            albumViewer.addPhotosRequested(albumViewer.album);
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

    PopupActionCancelDialog {
      id: trashDialog

      popupOriginX: -gu(16.5)
      popupOriginY: -gu(6)

      visible: false

      explanatoryText: "Selecting remove will remove this photo from this "
        + "album only."

      actionTitle: "Remove"

      onActionRequested: {
        album.removeSelectedMediaSources(gridCheckerboard.model);

        gridCheckerboard.unselectAll();
        gridCheckerboard.inSelectionMode = false;

        if (album.contentPageCount == 0) {
          trashModel.destroyAlbum(album);
          albumViewer.closeRequested(true);
          return;
        }

        // In the Album model, the last valid current page is the back cover.
        // However, in the UI, we want to stay on the content pages.  The -1 is
        // since the last content page will be on the right, but current page
        // must always be the left page.
        if (album.currentPage > album.lastContentPage - 1) {
          album.currentPage = album.lastContentPage - 1;
          albumSpreadViewer.setToAlbumCurrent();
        }
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()

      AlbumCollectionModel {
        id: trashModel
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
    z: 100

    onOpening: {
      // although this might be used by the page viewer, it too uses the grid's
      // models because you can walk the entire album from both
      model = gridCheckerboard.photoViewerModel;
    }

    onIndexChanged: {
      if (forGridView) {
        gridCheckerboard.ensureIndexVisible(index, false);
      } else {
        var page = album.getPageForMediaSource(photo);
        if (page) {
          album.currentPage = albumSpreadViewer.getLeftHandPageNumber(page.pageNumber);
          albumSpreadViewer.setToAlbumCurrent();
        }
      }
    }

    onCloseRequested: {
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
}

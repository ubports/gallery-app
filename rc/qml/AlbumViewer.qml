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

import QtQuick 2.0
import Gallery 1.0
import "../Capetown"
import "../js/Gallery.js" as Gallery
import "../js/GalleryUtility.js" as GalleryUtility
import "Components"
import "Utility"
import "Widgets"

Rectangle {
  id: albumViewer

  property Album album
  
  // Read-only
  property alias pagesPerSpread: albumSpreadViewer.pagesPerSpread
  property bool animationRunning: photoViewer.animationRunning ||
    albumSpreadViewer.isFlipping || removeCrossfadeAnimation.running ||
    albumSpreadViewerForTransition.freeze
  
  // When the user clicks the back button or pages back to the cover.
  signal closeRequested(bool stayOpen, int viewingPage)

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
    if (state == "pageView")
      gridCheckerboard.selection.leaveSelectionMode();
  }
  
  function crossfadeRemove() {
    removeCrossfadeAnimation.restart();
  }
  
  function fadeOutAndFlipRemove(flipToPage) {
    fadeOutAnimation.flipToPage = flipToPage;
    fadeOutAnimation.restart();
  }

  FadeOutAnimation {
    id: fadeOutAnimation
    
    property int flipToPage: 0
    
    target: isPortrait ? albumSpreadViewerForTransition.rightPageComponent :
                         albumSpreadViewerForTransition.leftPageComponent
    duration: 200
    easingType: Easing.Linear
    
    onRunningChanged: {
      if (running)
        return;
      
      flipTimer.restart()
      
      albumSpreadViewerForTransition.flipTo(flipToPage);
      albumSpreadViewer.flipTo(flipToPage);
    }
  }
  
  Timer {
    id: flipTimer
    
    interval: albumSpreadViewer.duration
    
    onTriggered: albumSpreadViewerForTransition.freeze = false
  }
  
  DissolveAnimation {
    id: removeCrossfadeAnimation
    
    duration: 200
    fadeOutTarget: albumSpreadViewerForTransition
    fadeInTarget: albumSpreadViewer
    easingType: Easing.Linear
    
    onRunningChanged: {
      if (running)
        return;
      
      albumSpreadViewerForTransition.freeze = false;
    }
  }
  
  function resetView(album) {
    albumViewer.album = album;

    state = ""; // Prevents the animation on gridView -> pageView from happening.
    state = "pageView";

    albumSpreadViewer.visible = true;
    chrome.resetVisibility(!Gallery.isSmallFormFactor());
    gridCheckerboard.visible = false;

    albumSpreadViewer.viewingPage = album.currentPage;
  }
  
  // Used for the cross-fade transition.
  AlbumSpreadViewer {
    id: albumSpreadViewerForTransition
    
    anchors.fill: parent
    album: albumViewer.album
    z: 100
    visible: freeze
    
    Connections {
      target: albumSpreadViewer
      
      onViewingPageChanged: {
        if (!albumSpreadViewerForTransition.freeze)
          albumSpreadViewerForTransition.flipTo(albumSpreadViewer.viewingPage);
      }
    }
    
    onFreezeChanged: {
      if (!freeze)
        flipTo(albumSpreadViewer.viewingPage);
    }
  }
  
  AlbumSpreadViewer {
    id: albumSpreadViewer

    anchors.fill: parent

    album: albumViewer.album
    
    // Keyboard focus while visible and viewer is not visible
    focus: !photoViewer.isPoppedUp && visible
    
    showCover: !albumSpreadViewerForTransition.freeze
    
    onPageFlipped: {
      if (!Gallery.isSmallFormFactor())
        chrome.show(false);
    }
    onPageReleased: {
      if (!Gallery.isSmallFormFactor())
        chrome.show(false);
    }
    
    Keys.onPressed: {
      if (event.key !== Qt.Key_Left && event.key !== Qt.Key_Right)
        return;

      var direction = (event.key === Qt.Key_Left ? -1 : 1);
      var destination = albumSpreadViewer.viewingPage +
          direction * albumSpreadViewer.pagesPerSpread;

      if (!albumSpreadViewer.isFlipping &&
          albumSpreadViewer.isPopulatedContentPage(destination)) {
        chrome.hide(true);
        
        albumSpreadViewerForTransition.flipTo(destination);
        
        event.accepted = true;
      }
    }
    
    SwipeArea {
      property real commitTurnFraction: 0.05

      // Normal press/click.
      function pressed(x, y) {
        var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
        if (!hit)
          return;
        
        // Handle add button.
        if (hit.objectName === "addButton")
          mediaSelector.show();
        
        if (!hit.mediaSource)
          return;
        
        if (gridCheckerboard.selection.inSelectionMode) {
          gridCheckerboard.selection.toggleSelection(hit.mediaSource);
        } else {
          photoViewer.forGridView = false;
          photoViewer.fadeOpen(hit.mediaSource);
        }
      }

      // Long press/right click.
      function alternativePressed(x, y) {
        var hit = albumSpreadViewer.hitTestFrame(x, y, parent);
        if (!hit || !hit.mediaSource)
          return;

        albumPagePhotoMenu.positionRelativeTo(hit.mediaSource);
        chrome.cyclePopup(albumPagePhotoMenu);
      }

      anchors.fill: parent

      enabled: !parent.isRunning
      
      onTapped: {
        if (rightButton)
          alternativePressed(x, y);
        else
          pressed(x, y);
      }
      onLongPressed: alternativePressed(x, y)
      
      onStartSwipe: {
        var direction = (leftToRight ? -1 : 1);
        albumSpreadViewer.destinationPage =
            albumSpreadViewer.viewingPage +
            direction * albumSpreadViewer.pagesPerSpread;

        // turn off chrome, allow the page flipper full screen
        chrome.hide(true);
      }

      onSwiping: {
        if (!albumSpreadViewer.isPopulatedContentPage(
            albumSpreadViewer.destinationPage)) {
          closeRequested(false, albumSpreadViewer.viewingPage);
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

    topExtraGutter: chrome.navbarHeight + getDeviceSpecific("photoGridTopMargin")
    bottomExtraGutter: chrome.toolbarHeight
    leftExtraGutter: getDeviceSpecific("photoGridLeftMargin")
    rightExtraGutter: getDeviceSpecific("photoGridRightMargin")

    visible: false
    
    selection: OrganicSelectionState {
      model: gridCheckerboard.model
    }
    
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
    
    onVisibleChanged: {
      if (!visible && album) {
        // When we hide this, we need to ensure the album is closed if it's empty.
        if (album.containedCount == 0)
          album.closed = true;
      }
    }
    
    // When album is empty, show the add button
    MattedPhotoPreview {
      visible: album !== null && album.containedCount == 0
      
      anchors.top: parent.top
      anchors.left: parent.left
      anchors.topMargin: gridCheckerboard.topExtraGutter
      anchors.leftMargin: gridCheckerboard.leftExtraGutter
      substituteSource: "Components/AlbumInternals/img/album-add.png"
      
      MouseArea {
        anchors.fill: parent
        onClicked: mediaSelector.show()
      }
    }
  }

  MouseArea {
    id: chromeShowArea

    width: parent.width
    height: chrome.toolbarHeight
    anchors.bottom: parent.bottom
    enabled: (Gallery.isSmallFormFactor() && albumViewer.state == "pageView" &&
              chrome.state == "hidden")
    onClicked: chrome.show(true)
  }
  MouseArea {
    id: chromeHideArea

    anchors.fill: parent
    enabled: (Gallery.isSmallFormFactor() && albumViewer.state == "pageView" &&
              chrome.state == "shown")
    onClicked: chrome.hide(true)
  }

  ViewerChrome {
    id: chrome

    anchors.fill: parent

    state: "hidden"
    visible: false

    autoHideWait: 0
    
    pagesPerSpread: albumSpreadViewer.pagesPerSpread
    viewingPage: albumSpreadViewer.viewingPage

    inSelectionMode: gridCheckerboard.selection.inSelectionMode

    hasSelectionOperationsButton: gridCheckerboard.selection.inSelectionMode
    onSelectionOperationsButtonPressed: cyclePopup(selectionMenu)

    toolbarsAreTranslucent: (albumViewer.state == "gridView" ||
                             Gallery.isSmallFormFactor())
    toolbarsAreTextured: (albumViewer.state == "gridView" ||
                          Gallery.isSmallFormFactor())

    navbarHasStateButton: true
    navbarSelectedStateButtonIconFilename: (albumViewer.state == "pageView"
      ? "../img/icon-grid-view-active.png"
      : "../img/icon-album-view-active.png")
    navbarDeselectedStateButtonIconFilename: (albumViewer.state == "pageView"
      ? "../img/icon-grid-view-inactive.png"
      : "../img/icon-album-view-inactive.png")

    toolbarHasFullIconSet: false
    toolbarHasAlbumOperationsButton: false
    toolbarHasPageIndicator: (albumViewer.state == "pageView" &&
                              !Gallery.isSmallFormFactor())
    toolbarPageIndicatorAlbum: albumViewer.album

    popups: [ albumViewerOptionsMenu, albumViewerShareMenu,
      selectionMenu, trashDialog, albumPagePhotoMenu, trashFromAlbumPageDialog,
      albumTrashDialog ]

    onPageIndicatorPageSelected: {
      chrome.hide(true);
      albumSpreadViewer.flipTo(page);
    }

    onStateButtonPressed: {
      albumViewer.state = (albumViewer.state == "pageView" ? "gridView" : "pageView");
    }

    onSelectionDoneButtonPressed: gridCheckerboard.selection.leaveSelectionMode()

    onReturnButtonPressed: {
      gridCheckerboard.selection.leaveSelectionMode();

      closeRequested(album.containedCount > 0, albumSpreadViewer.viewingPage);
    }

    onMoreOperationsButtonPressed: cyclePopup(albumViewerOptionsMenu)
    onShareOperationsButtonPressed: cyclePopup(albumViewerShareMenu)
    onTrashOperationButtonPressed: cyclePopup(trashDialog)

    SelectionMenu {
      id: selectionMenu

      selection: OrganicSelectionState {
        model: gridCheckerboard.model
      }

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
          
          case "onDeleteAlbum":
            chrome.cyclePopup(albumTrashDialog);
          break;
        }
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()

      visible: false;
    }

    GenericShareMenu {
      id: albumViewerShareMenu

      popupOriginX: -gu(9)
      popupOriginY: -gu(6)

      onActionInvoked: {
        switch (name) {
          case "onQuickShare": {          
          if (albumViewer.state == "gridView") {
              for (var i = 0; i < gridCheckerboard.selection.model.count; i++) {
                var img = gridCheckerboard.selection.model.getAt(i);
                
                if (gridCheckerboard.selection.model.isSelected(gridCheckerboard.selection.model.getAt(i))) {
                  APP.run_command("xdg-open" , gridCheckerboard.selection.model.getAt(i).path);
                }
              }
              break;
            }
          }
        }
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()

      visible: false
    }
    
    // When delete is invoked from grid view
    DeleteRemoveDialog {
      id: trashDialog

      // internal
      function finishRemove() {
        gridCheckerboard.selection.leaveSelectionMode();
        
        // In the Album model, the last valid current page is the back cover.
        // However, in the UI, we want to stay on the content pages.
        if (album.currentPage > album.lastPopulatedContentPage - 1) {
          album.currentPage = albumSpreadViewer.getLeftHandPageNumber(
                album.lastPopulatedContentPage);
          albumSpreadViewer.viewingPage = album.lastPopulatedContentPage;
        }
      }
      
      action0Title: "Remove from album"
      action1Title: "Delete photo"
      
      popupOriginX: -gu(16.5)
      popupOriginY: -gu(6)

      visible: false

      onRemoveRequested: {
        album.removeSelectedMediaSources(gridCheckerboard.selection.model);
        
        finishRemove(false);
      }

      onDeleteRequested: {
        gridCheckerboard.selection.model.destroySelectedMedia();

        finishRemove(false);
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
            APP.run_command("xdg-open" , mediaSource.path);
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
      
      action0Title: "Remove from album"
      action1Title: "Delete photo"
      
      // internal
      // media: photo to remove/delete
      // deleteMedia: if true, the backing file will be deleted
      function removeOrDelete(media, deleteMedia) {
        albumSpreadViewerForTransition.freeze = true;
        album.removeMediaSource(media);
        
        // Display the proper animation for this case.
        if (albumSpreadViewer.viewingPage > album.lastPopulatedContentPage) {
          // In the Album model, the last valid current page is the back cover.
          // However, in the UI, we want to stay on the content pages.
          fadeOutAndFlipRemove(isPortrait ? album.lastPopulatedContentPage :
                                            album.lastPopulatedContentPage - 1);
        } else {
          // For most album situations, just fade the old page into the new one.
          crossfadeRemove();
        }
        
        if (deleteMedia)
          gridCheckerboard.model.destroyMedia(media);
      }
      
      onRemoveRequested: removeOrDelete(media, false)
      
      onDeleteRequested: removeOrDelete(media, true)
      
      onPopupInteractionCompleted: chrome.hideAllPopups()
      
      AlbumCollectionModel {
        id: trashFromAlbumPageModel
      }
    }
    
    // Delete album from album view.
    DeleteOrDeleteWithContentsDialog {
      id: albumTrashDialog
      
      visible: false
      
      deleteTitle: "Delete album"
      deleteWithContentsTitle: "Delete album + contents"
      
      popupOriginX: -gu(2)
      popupOriginY: -gu(6)
      
      onDeleteRequested: {
        albumCollectionModel.destroyAlbum(albumViewer.album);
        albumViewer.closeRequested(true, -1);
      }
      
      onDeleteWithContentsRequested: {
        // Remove contents.
        var list = albumViewer.album.allMediaSources;
        for (var i = 0; i < list.length; i++)
          gridCheckerboard.model.destroyMedia(list[i]);
        
        // Remove album.
        albumCollectionModel.destroyAlbum(albumViewer.album);
        albumViewer.closeRequested(true, -1);
      }
      
      AlbumCollectionModel {
        id: albumCollectionModel
      }
      
      onPopupInteractionCompleted: state = "hidden"
    }
  }
  
  PopupPhotoViewer {
    id: photoViewer
    
    // true if the grid view component is using the photo viewer, false if the
    // album spread viewer is using it ... this should be set prior to 
    // opening the viewer
    property bool forGridView
    
    album: albumViewer.album
    
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
        var page = albumViewer.album.getPageForMediaSource(photo);
        if (page >= 0) {
          albumViewer.album.currentPage = albumSpreadViewer.getLeftHandPageNumber(page);
          albumSpreadViewer.viewingPage = isPortrait? page : albumViewer.album.currentPage;
        }
      }
    }

    onCloseRequested: {
      if (forGridView) {
        var rect = gridCheckerboard.getRectOfItemAt(index, photoViewer);
        if (rect)
          animateClosed(rect, false);
        else
          close();
      } else {
        fadeClosed();
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

        chrome.hide(true);
        albumSpreadViewer.flipTo(firstChangedSpread);
      }
    }
  }
}

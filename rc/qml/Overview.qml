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
  id: overview
  objectName: "overview"
  
  anchors.fill: parent

  function getRectOfAlbumPreview(album, relativeTo) {
    return albumsCheckerboard.getRectOfItemAt(albumsCheckerboard.model.indexOf(album), relativeTo);
  }

  state: "eventView"

  states: [
    State { name: "eventView"; },
    State { name: "albumView"; }
  ]

  transitions: [
    Transition { from: "eventView"; to: "albumView";
      DissolveAnimation { fadeOutTarget: eventsSheet; fadeInTarget: albumsCheckerboard; }
    },
    Transition { from: "albumView"; to: "eventView";
      DissolveAnimation { fadeOutTarget: albumsCheckerboard; fadeInTarget: eventsSheet; }
    }
  ]

  GalleryOverviewNavigationBar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    onAddCreateOperationButtonPressed: {
      if (albumViewSwitcher.state == "tab0_active")
        navStack.switchToMediaSelector();
    }
    
    BinaryTabGroup {
      id: albumViewSwitcher
      objectName: "albumViewSwitcher"
      
      x: parent.width / 2 - width / 2
      y: 4
      
      tab0_title: "Albums"
      tab1_title: "Events"
      
      state: "tab1_active"
      
      visible: !eventsCheckerboard.inSelectionMode
      
      onTab0_activated: {
        overview.state = "albumView";
      }
      
      onTab1_activated: {
        overview.state = "eventView"
      }
    }
    
    GallerySecondaryPushButton {
      id: deselect
      objectName: "deselect"
      
      anchors.right: cancelSelecting.left
      anchors.rightMargin: 16
      anchors.verticalCenter: parent.verticalCenter

      title: "deselect"

      visible: eventsCheckerboard.selectedCount > 0
      
      onPressed: {
        if (albumPicker.state == "shown") {
          albumPicker.state = "hidden";
          return;
        }
        
        eventsCheckerboard.unselectAll();
      }
    }
    
    GalleryPrimaryPushButton {
      id: cancelSelecting
      objectName: "cancelSelecting"
      
      anchors.right: parent.right
      anchors.rightMargin: 16
      anchors.verticalCenter: parent.verticalCenter

      title: "done"

      visible: eventsCheckerboard.inSelectionMode
      
      onPressed: {
        if (albumPicker.state == "shown") {
          albumPicker.state = "hidden";
          return;
        }

        eventsCheckerboard.state = "normal";
        eventsCheckerboard.unselectAll();
      }
    }
  }
  
  Rectangle {
    id: eventsSheet
    
    anchors.top: navbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    // if switched away from or to, always move back to checkerboard
    onVisibleChanged: {
      eventsCheckerboard.visible = true;
      eventTimeline.visible = false;
    }
    
    EventCheckerboard {
      id: eventsCheckerboard
      objectName: "eventsCheckerboard"
      
      anchors.fill: parent
      
      visible: true
      allowSelection: true
      
      property variant photoViewerModel: MediaCollectionModel {
      }
      
      MouseArea {
        anchors.fill: parent
        
        onClicked: {
          if (albumPicker.state == "shown") {
            albumPicker.state = "hidden";
            return;
          }
        }
        
        visible: albumPicker.state == "shown"
      }
      
      onActivated: {
        if (objectModel.typeName == "MediaSource") {
          photoViewer.animateOpen(object, activatedRect);
        } else {
          // Event marker
          visible = false;
          eventTimeline.visible = true;
        }
      }
    }
    
    EventTimeline {
      id: eventTimeline
      
      anchors.fill: parent
      
      visible: false
      clip: true
      
      onActivated: {
        // Event marker
        visible = false;
        eventsCheckerboard.visible = true;
      }
    }
  }
  
  Checkerboard {
    id: albumsCheckerboard
    objectName: "albumsCheckerboard"
    
    anchors.top: navbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 24
    anchors.bottomMargin: 0
    anchors.leftMargin: 22
    anchors.rightMargin: 22
    
    widthSansStroke: 412
    heightSansStroke: 312
    widthWithStroke: 388
    heightWithStroke: 288
    
    visible: false
    allowSelection: false
    
    model: AlbumCollectionModel {
    }
    
    delegate: AlbumPreviewComponent {
      album: modelData.album
    }
    
    onActivated: navStack.switchToAlbumViewer(object, activatedRect)
  }

  AlbumPickerPopup {
    id: albumPicker
    objectName: "albumPicker"

    y: parent.height - height - toolbar.height
    x: toolbar.albumOperationsPopupX - width

    state: "hidden"

    designated_model: AlbumCollectionModel {
    }

    onNewAlbumRequested: {
      eventsCheckerboard.model.createAlbumFromSelected();
      eventsCheckerboard.state = "normal";
      eventsCheckerboard.unselectAll();
      state = "hidden"
    }

    onSelected: {
      album.addSelectedMediaSources(eventsCheckerboard.model);
      eventsCheckerboard.state = "normal";
      eventsCheckerboard.unselectAll();
      state = "hidden"
    }
  }
  
  GalleryStatusBar {
    id: statusBar
    objectName: "statusBar"
    
    z: 10
    anchors.bottom: parent.bottom

    visible: (eventsCheckerboard.inSelectionMode &&
      eventsCheckerboard.selectedCount == 0)
    
    statusText: "Select photos or movieclip(s)"
  }

  GalleryStandardToolbar {
    id: toolbar
    objectName: "toolbar"

    hasReturnButton: false
    isTranslucent: true
    background: "lightBlue"

    z: 10
    anchors.bottom: parent.bottom

    visible: (eventsCheckerboard.inSelectionMode &&
      eventsCheckerboard.selectedCount > 0)

    onAlbumOperationsButtonPressed: { albumPicker.flipVisibility(); }
  }

  PopupPhotoViewer {
    id: photoViewer

    anchors.fill: parent
    z: 100
    
    onOpening: {
      model = eventsCheckerboard.photoViewerModel;
    }
    
    onPhotoChanged: {
      if (photo && eventsCheckerboard.model) {
        eventsCheckerboard.ensureIndexVisible(
          eventsCheckerboard.model.indexOf(photo)
        );
      }
    }

    onCloseRequested: {
      var thumbnailRect =
        eventsCheckerboard.getRectOfItemAt(eventsCheckerboard.model.indexOf(photo), photoViewer);
      if (thumbnailRect)
        animateClosed(thumbnailRect);
      else
        close();
    }
  }
}

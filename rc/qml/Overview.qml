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
  id: overview
  objectName: "overview"
  
  property Rectangle glass: overviewGlass
  
  anchors.fill: parent

  function getRectOfAlbumPreview(album, relativeTo) {
    return albumsCheckerboard.getRectOfItemAt(albumsCheckerboard.model.indexOf(album), relativeTo);
  }
  
  function showAlbumPreview(album, show) {
    var delegate = albumsCheckerboard.getDelegateInstanceAt(albumsCheckerboard.model.indexOf(album));
    if (delegate)
      delegate.visible = show;
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

  Image {
    source: "../img/overview-background.png"
  }

  GalleryOverviewNavigationBar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    
    visible: !eventsCheckerboard.inSelectionMode

    onAddCreateOperationButtonPressed: {
      if (albumViewSwitcher.state == "tab0_active")
        navStack.switchToMediaSelector();
    }
    
    BinaryTabGroup {
      id: albumViewSwitcher
      objectName: "albumViewSwitcher"
      
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      
      tab0_title: "Albums"
      tab1_title: "Events"
      
      state: "tab1_active"
      
      onTab0_activated: {
        overview.state = "albumView";
      }
      
      onTab1_activated: {
        overview.state = "eventView"
      }
    }
  }
  
  Item {
    id: eventsSheet
    
    anchors.fill: parent
    anchors.topMargin: gu(2) + navbar.height
    anchors.bottomMargin: gu(0)
    anchors.leftMargin: gu(2)
    anchors.rightMargin: gu(2)
    
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
      color: "transparent"
      allowSelection: true
      
      property variant photoViewerModel: MediaCollectionModel {
        monitored: true
      }
      
      onActivated: {
        if (objectModel.typeName == "MediaSource") {
          var photoRect = GalleryUtility.translateRect(activatedRect, eventsCheckerboard, photoViewer);
          photoViewer.animateOpen(object, photoRect);
        } else {
          // Event
          eventTimelineTransition.toTimeline(object);
        }
      }
    }
    
    EventTimelineTransition {
      id: eventTimelineTransition
      
      anchors.fill: parent
      
      visible: false
      clip: true
      
      checkerboard: eventsCheckerboard
      timeline: eventTimeline
      
      delegate: EventCheckerboardDelegate {
        ownerName: "EventTimelineTransition"
      }
    }
    
    EventTimeline {
      id: eventTimeline
      
      anchors.fill: parent
      
      visible: false
      clip: true
      
      onActivated: {
        // Event card activated
        eventTimelineTransition.toOverview(event);
      }

      onTimedOut: eventTimelineTransition.backToOverview()
    }
  }
  
  Checkerboard {
    id: albumsCheckerboard
    objectName: "albumsCheckerboard"
    
    color: "transparent"

    anchors.top: navbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: gu(2)
    anchors.bottomMargin: gu(0)
    anchors.leftMargin: gu(2)
    anchors.rightMargin: gu(2)
    
    itemWidth: gu(28)
    itemHeight: gu(33)
    gutterWidth: gu(11)
    gutterHeight: gu(8)
    
    visible: false
    allowSelection: false
    allowActivation: false
    
    model: AlbumCollectionModel {
    }
    
    delegate: AlbumPreviewComponent {
      album: modelData.album

      SwipeArea {
        anchors.fill: parent
        
        enabled: !parent.isRunning
        
        onTapped: {
          var rect = GalleryUtility.getRectRelativeTo(parent, albumsCheckerboard);
          albumsCheckerboard.activated(modelData.object, modelData.model, rect);
        }
      }
    }
    
    onActivated: {
      var albumRect = GalleryUtility.translateRect(activatedRect, albumsCheckerboard, overview);
      navStack.switchToAlbumViewer(object, albumRect);
    }
  }

  ViewerChrome {
    id: chrome

    z: 10
    anchors.fill: parent

    fadeDuration: 0
    autoHideWait: 0
    
    hasSelectionOperationsButton: true
    
    inSelectionMode: true
    visible: eventsCheckerboard.inSelectionMode
    
    property variant popups: [ selectionOperationsMenu ]
    
    onSelectionOperationsButtonPressed: {
      cyclePopup(selectionOperationsMenu);
    }
    
    onSelectionDoneButtonPressed: {
      eventsCheckerboard.state = "normal";
      eventsCheckerboard.unselectAll();
    }

    onNewAlbumPicked: {
      eventsCheckerboard.model.createAlbumFromSelected();
      eventsCheckerboard.state = "normal";
      eventsCheckerboard.unselectAll();
    }

    onAlbumPicked: {
      album.addSelectedMediaSources(eventsCheckerboard.model);
      eventsCheckerboard.state = "normal";
      eventsCheckerboard.unselectAll();
    }
    
    PopupMenu {
      id: selectionOperationsMenu
      
      popupOriginX: gu(3.5)
      popupOriginY: -gu(6)
      
      visible: false
      state: "hidden"
      
      model: ListModel {
        ListElement {
          title: "Select All"
          action: "SelectAll"
          hasBottomBorder: true
        }
        
        ListElement {
          title: "Select None"
          action: "SelectNone"
        }
      }
      
      onPopupInteractionCompleted: {
        hideAllPopups();
      }
      
      onActionInvoked: {
        switch (name) {
          case "SelectAll":
            eventsCheckerboard.selectAll();
          break;
          
          case "SelectNone":
            eventsCheckerboard.unselectAll();
          break;
        }
      }
    }
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
        eventsCheckerboard.ensureIndexVisible(eventsCheckerboard.model.indexOf(photo),
          false);
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
  
  Rectangle {
    id: overviewGlass
    
    anchors.fill: parent
    
    color: "black"
    opacity: 0.0
  }
}

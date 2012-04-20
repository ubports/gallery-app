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

  signal albumSelected(variant album, variant thumbnailRect)

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
    cache: true
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
    anchors.topMargin: navbar.height

    // if switched away from or to, always move back to checkerboard
    onVisibleChanged: {
      eventsCheckerboard.visible = true;
      eventTimeline.visible = false;
    }

    EventCheckerboard {
      id: eventsCheckerboard
      objectName: "eventsCheckerboard"

      anchors.fill: parent

      topExtraGutter: gu(2)
      bottomExtraGutter: gu(0)
      leftExtraGutter: gu(2)
      rightExtraGutter: gu(2)

      visible: true
      color: "transparent"
      allowSelection: true

      property variant photoViewerModel: MediaCollectionModel {
        monitored: true
      }

      onActivated: {
        if (objectModel.typeName == "MediaSource") {
          var photoRect = GalleryUtility.translateRect(activatedRect, eventsCheckerboard, photoViewer);
          photoViewer.animateOpen(object, photoRect, true);
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
    }

    EventTimeline {
      id: eventTimeline

      anchors.fill: parent

      topExtraGutter: gu(2)
      bottomExtraGutter: gu(0)
      leftExtraGutter: gu(2)
      rightExtraGutter: gu(2)

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

    topExtraGutter: gu(2)
    bottomExtraGutter: gu(0)
    leftExtraGutter: gu(2)
    rightExtraGutter: gu(2)

    itemWidth: gu(28)
    itemHeight: gu(33)
    gutterWidth: gu(11)
    gutterHeight: gu(8)

    visible: false
    allowSelection: true
    allowActivation: false

    model: AlbumCollectionModel {
    }

    delegate: CheckerboardDelegate {
      property real commitFraction: 0.05

      // internal
      property bool validSwipe: false

      checkerboard: albumsCheckerboard

      onSwipeStarted: {
        validSwipe = ((leftToRight && !album.closed) || (!leftToRight && album.closed));
      }

      onSwiping: {
        if (!validSwipe)
          return;

        var availableDistance = (leftToRight) ? (width - start) : start;
        var fraction = Math.max(0, Math.min(1, distance / availableDistance));

        albumThumbnail.openFraction = (leftToRight ? 1 - fraction : fraction);
      }

      onSwiped: {
        if (!validSwipe)
          return;

        var fraction = (leftToRight
          ? 1 - albumThumbnail.openFraction
          : albumThumbnail.openFraction);
        if ((leftToRight && fraction >= commitFraction)
          || (!leftToRight && fraction < commitFraction))
          albumThumbnail.close();
        else
          albumThumbnail.open();
      }

      content: AlbumPreviewComponent {
        id: albumThumbnail

        property real maxAddScale: 0.5

        album: modelData.album

        z: (isFlipping ? 10 : 0)

        // Scale from 1 to 1 + maxAddScale and back to 1 as openFraction goes
        // from 0 to 0.5 to 1.
        scale: 1 + maxAddScale - Math.abs((openFraction - 0.5) * maxAddScale * 2)
      }
    }

    onActivated: {
      var albumRect = GalleryUtility.translateRect(activatedRect, albumsCheckerboard, overview);
      albumSelected(object, albumRect);
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
    visible: eventsCheckerboard.inSelectionMode || albumsCheckerboard.inSelectionMode

    property variant popups: [ selectionOperationsMenu, popupAlbumPicker ]

    onSelectionOperationsButtonPressed: cyclePopup(selectionOperationsMenu);

    onSelectionDoneButtonPressed: {
      eventsCheckerboard.unselectAll();
      eventsCheckerboard.inSelectionMode = false;

      albumsCheckerboard.unselectAll();
      albumsCheckerboard.inSelectionMode = false;
    }

    onAlbumOperationsButtonPressed: {
      if (eventsCheckerboard.inSelectionMode)
        cyclePopup(popupAlbumPicker);
    }

    PopupAlbumPicker {
      id: popupAlbumPicker

      popupOriginX: -gu(17.5)
      popupOriginY: -gu(6)

      onPopupInteractionCompleted: {
        chrome.hideAllPopups();
        eventsCheckerboard.inSelectionMode = false;
      }

      onNewAlbumRequested: {
        eventsCheckerboard.model.createAlbumFromSelected();
        eventsCheckerboard.unselectAll();
      }

      onAlbumPicked: {
        album.addSelectedMediaSources(eventsCheckerboard.model);
        eventsCheckerboard.unselectAll();
      }

      visible: false
      state: "hidden"
    }

    SelectionMenu {
      id: selectionOperationsMenu
      
      checkerboard: (eventsCheckerboard.inSelectionMode ? eventsCheckerboard : albumsCheckerboard)
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
        animateClosed(thumbnailRect, true);
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

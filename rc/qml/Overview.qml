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

  signal addAlbumRequested()
  signal editAlbumRequested(variant album, variant thumbnailRect)
  signal albumSelected(variant album, variant thumbnailRect)
  signal editPhotoRequested(variant photo)

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

  Item {
    id: eventsSheet

    anchors.fill: parent

    // if switched away from or to, always move back to checkerboard
    onVisibleChanged: {
      eventsCheckerboard.visible = true;
      eventTimeline.visible = false;
    }

    EventCheckerboard {
      id: eventsCheckerboard
      objectName: "eventsCheckerboard"

      anchors.fill: parent
      header: Item {
        id: eventsEmptyScrollSpace

        height: navbar.height;
        width: parent.width;
      }

      topExtraGutter: gu(2)
      bottomExtraGutter: gu(0)
      leftExtraGutter: gu(2)
      rightExtraGutter: gu(2)

      visible: true
      allowSelectionModeChange: true

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

      onMovementStarted: {
        scrollOrchestrator.viewMovementStarted(contentY);
      }

      onContentYChanged: {
        scrollOrchestrator.viewScrolled(contentY);
      }

      onMovementEnded: {
        scrollOrchestrator.viewMovementEnded(contentY);
      }

      onVisibleChanged: {
        if (visible) {
          // If we just became visible, it's possible that our scroll position
          // changed due to moving in/out of the timeline view. Let the scroll
          // orchestrator know this so it can keep its state consistent.
          scrollOrchestrator.viewScrolled(contentY);
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
      navigationBar: navbar
    }

    EventTimeline {
      id: eventTimeline

      anchors.fill: parent
      header: Item {
        id: timelineEmptyScrollSpace

        height: navbar.height;
        width: parent.width;
      }

      topExtraGutter: gu(0)
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

      onMovementStarted: scrollOrchestrator.viewMovementStarted(contentY)

      onMovementEnded: scrollOrchestrator.viewMovementEnded(contentY)

      onContentYChanged: scrollOrchestrator.viewScrolled(contentY)
    }
  }

  Checkerboard {
    id: albumsCheckerboard
    objectName: "albumsCheckerboard"

    anchors.fill: parent
    header: Item {
      id: albumsEmptyScrollSpace

      height: navbar.height;
      width: parent.width;
    }

    topExtraGutter: gu(2)
    bottomExtraGutter: gu(0)
    leftExtraGutter: gu(2)
    rightExtraGutter: gu(2)

    itemWidth: gu(28)
    itemHeight: gu(33)
    gutterWidth: gu(11)
    gutterHeight: gu(8)

    visible: false
    allowSelectionModeChange: true
    singleSelectionOnly: true
    allowActivation: false

    model: AlbumCollectionModel {
    }

    delegate: CheckerboardDelegate {
      property real commitFraction: 0.05

      // internal
      property bool validSwipe: false

      z: (albumThumbnail.isFlipping ? 10 : 0)

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

        // Scale from 1 to 1 + maxAddScale and back to 1 as openFraction goes
        // from 0 to 0.5 to 1.
        scale: 1 + maxAddScale - Math.abs((openFraction - 0.5) * maxAddScale * 2)
      }
    }

    onActivated: {
      var albumRect = GalleryUtility.translateRect(activatedRect, albumsCheckerboard, overview);
      albumSelected(object, albumRect);
    }

    onMovementStarted: {
      scrollOrchestrator.viewMovementStarted(contentY);
    }

    onContentYChanged: {
      scrollOrchestrator.viewScrolled(contentY);
    }

    onMovementEnded: {
      scrollOrchestrator.viewMovementEnded(contentY);
    }
  }

  NavbarScrollOrchestrator {
    id: scrollOrchestrator

    navigationBar: navbar

    onInitialized: {
      eventsCheckerboard.contentY = 0;
      albumsCheckerboard.contentY = 0;
    }
  }

  GalleryOverviewNavigationBar {
    id: navbar
    objectName: "navbar"

    y: 0
    anchors.left: parent.left
    anchors.right: parent.right

    visible: !eventsCheckerboard.inSelectionMode

    onAddCreateOperationButtonPressed: {
      if (albumViewSwitcher.state == "tab0_active")
        overview.addAlbumRequested();
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
        scrollOrchestrator.reset();
        overview.state = "albumView";
      }

      onTab1_activated: {
        scrollOrchestrator.reset();
        overview.state = "eventView"
      }
    }
  }

  ViewerChrome {
    id: chrome

    z: 10
    anchors.fill: parent

    fadeDuration: 0
    autoHideWait: 0

    hasSelectionOperationsButton: eventsCheckerboard.inSelectionMode
    toolbarHasAlbumOperationsButton: false

    inSelectionMode: true
    visible: eventsCheckerboard.inSelectionMode || albumsCheckerboard.inSelectionMode

    popups: (eventsCheckerboard.inSelectionMode
      ? [ selectionOperationsMenu, photoTrashDialog ]
      : [ albumOptionsMenu, albumTrashDialog ])

    onSelectionOperationsButtonPressed: {
      if (eventsCheckerboard.inSelectionMode)
        cyclePopup(selectionOperationsMenu);
    }

    onMoreOperationsButtonPressed: {
      if (!eventsCheckerboard.inSelectionMode)
        cyclePopup(albumOptionsMenu);
    }

    onTrashOperationButtonPressed: {
      if (eventsCheckerboard.inSelectionMode)
        cyclePopup(photoTrashDialog);
      else
        cyclePopup(albumTrashDialog);
    }

    onSelectionDoneButtonPressed: {
      eventsCheckerboard.unselectAll();
      eventsCheckerboard.inSelectionMode = false;

      albumsCheckerboard.unselectAll();
      albumsCheckerboard.inSelectionMode = false;
    }

    SelectionMenu {
      id: selectionOperationsMenu

      checkerboard: (eventsCheckerboard.inSelectionMode ? eventsCheckerboard : albumsCheckerboard)

      onPopupInteractionCompleted: chrome.hideAllPopups()
    }

    PopupMenu {
      id: albumOptionsMenu

      popupOriginX: -gu(1.5)
      popupOriginY: -gu(6)

      visible: false

      model: ListModel {
        ListElement {
          title: "Edit"
          action: "onEdit"
        }
      }

      onActionInvoked: {
        // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you
        // edit a switch statement in QML.  The short version is: use braces
        // always.
        switch (name) {
          case "onEdit": {
            var album = albumsCheckerboard.singleSelectedItem;
            overview.editAlbumRequested(album, getRectOfAlbumPreview(album, overview));

            albumsCheckerboard.unselectAll();
            albumsCheckerboard.inSelectionMode = false;
            break;
          }
        }
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()
    }

    DeleteDialog {
      id: albumTrashDialog

      popupOriginX: -gu(16.5)
      popupOriginY: -gu(6)

      visible: false

      explanatoryText: "Selecting remove will remove this album only- the "
        + "contents of the album will remain."

      actionTitle: "Remove"

      onDeleteRequested: {
        var album = albumsCheckerboard.singleSelectedItem;
        albumsCheckerboard.model.destroyAlbum(album);

        chrome.hideAllPopups(); // Have to do here since our popup list changes
                                // based on selection mode.
        albumsCheckerboard.unselectAll();
        albumsCheckerboard.inSelectionMode = false;
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()
    }

    DeleteDialog {
      id: photoTrashDialog

      popupOriginX: -gu(16.5)
      popupOriginY: -gu(6)

      visible: false

      onDeleteRequested: {
        eventsCheckerboard.model.destroySelectedMedia();

        chrome.hideAllPopups(); // Have to do here since our popup list changes
                                // based on selection mode.
        eventsCheckerboard.unselectAll();
        eventsCheckerboard.inSelectionMode = false;
      }

      onPopupInteractionCompleted: chrome.hideAllPopups()
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

    onEditRequested: overview.editPhotoRequested(photo)

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

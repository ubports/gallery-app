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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

// The ViewerChrome object wraps toolbars and other chrome elements into one
// easy to use, standardized object.  You'll probably want to fill the screen
// with it, as it places its chrome children at its edges.
Item {
  id: wrapper

  property int fadeDuration: 200
  property alias autoHideWait: autoHideTimer.interval // 0 to disable auto-hide.

  property variant popups: [ ]

  property bool hasLeftNavigationButton: false
  property bool hasRightNavigationButton: false

  property bool toolbarsAreTranslucent: true

  property string navbarTitle
  property bool navbarHasStateButton: false
  property bool navbarHasCancelSelectionButton: false

  property bool toolbarHasFullIconSet: true
  property bool toolbarHasMainIconsWhenSelecting: true

  property bool inSelectionMode: false

  // signals sent from the entire chrome ensemble
  signal hidePopups();

  // Pass-throughs from the navbar.
  property alias navbarHeight: navbar.height
  property alias navbarStateButtonIconFilename: navbar.stateButtonIconFilename
  property alias navbarSelectionDoneButtonTitle: navbar.selectionDoneButtonTitle
  signal returnButtonPressed()
  signal stateButtonPressed()
  signal selectionDoneButtonPressed()
  signal cancelSelectionButtonPressed()

  // Pass-throughs from the toolbar.
  property alias toolbarHeight: toolbar.height
  property alias toolbarHasPageIndicator: toolbar.hasPageIndicator
  property alias toolbarPageIndicatorAlbum: toolbar.pageIndicatorAlbum
  signal pageIndicatorPageSelected(int pageNumber)
  signal moreOperationsButtonPressed()
  signal shareOperationsButtonPressed()

  // Pass-throughs from the left/right nav buttons.
  signal leftNavigationButtonPressed()
  signal rightNavigationButtonPressed()

  // Pass-throughs from the album picker.
  property alias albumPickerDesignatedModel: albumPicker.designated_model
  signal albumPicked(variant album)
  signal newAlbumPicked()

  // private properties -- don't touch these from outside this component
  property bool active: false

  visible: false
  state: "hidden"

  states: [
    State { name: "shown"; },
    State { name: "hidden"; }
  ]

  transitions: [
    Transition { from: "shown"; to: "hidden";
      FadeOutAnimation { target: wrapper; duration: fadeDuration; }
    },
    Transition { from: "hidden"; to: "shown";
      FadeInAnimation { target: wrapper; duration: fadeDuration; }
    }
  ]

  function flipVisibility() {
      state = (state == "shown" ? "hidden" : "shown");
  }

  function resetVisibility(visibility) {
    state = ""; // To prevent animation.
    state = (visibility ? "shown" : "hidden");
    visible = visibility;
  }
  
  function show() {
    state = "shown";
  }
  
  function hide() {
    state = "hidden";
  }
  
  function cyclePopup(target) {
    if (!target.visible)
      hideAllPopups();

    target.flipVisibility();

    this.active = target.visible;
  }

  function hideAllPopups() {
    for (var i = 0; i < popups.length; i++)
      popups[i].state = "hidden";

    this.active = false;
  }

  onVisibleChanged: {
    albumPicker.resetVisibility(false);

    wrapper.hidePopups();

    if (visible)
      autoHideTimer.startAutoHide();
  }

  onActiveChanged: {
    if (active)
      autoHideTimer.stop();
    else
      autoHideTimer.startAutoHide();
  }

  function cancelActivity() {
    this.hideAllPopups();
    albumPicker.state = "hidden";
  }

  MouseArea {
    id: cancelArea

    anchors.fill: parent
    z:16

    visible: chrome.active

    onClicked: chrome.cancelActivity()
  }

  ViewerNavigationButton {
    id: leftNavButton

    is_forward: false

    visible: wrapper.hasLeftNavigationButton && !wrapper.inSelectionMode

    x: gu(1.5)
    y: 2 * parent.height / 3
    
    onPressed: {
      autoHideTimer.startAutoHide();
      leftNavigationButtonPressed();
    }
  }

  ViewerNavigationButton {
    id: rightNavButton

    is_forward: true

    visible: wrapper.hasRightNavigationButton && !wrapper.inSelectionMode

    x: parent.width - width - gu(1.5)
    y: 2 * parent.height / 3
    
    onPressed: {
      autoHideTimer.startAutoHide();
      rightNavigationButtonPressed();
    }
  }

  AlbumPickerPopup {
    id: albumPicker

    anchors.bottom: toolbar.top
    x: toolbar.albumOperationsPopupX - width

    designated_model: AlbumCollectionModel {
    }

    onSelected: {
      albumPicked(album);
      state = "hidden";
    }

    onNewAlbumRequested: {
      newAlbumPicked();
      state = "hidden";
    }
  }

  GalleryStandardNavbar {
    id: navbar

    anchors.top: parent.top

    background: (!wrapper.inSelectionMode ? "white" : "lightBlue")
    isTranslucent: (!wrapper.inSelectionMode ? wrapper.toolbarsAreTranslucent : false)
    
    hasReturnButton: !wrapper.inSelectionMode
    hasStateButton: wrapper.navbarHasStateButton && !wrapper.inSelectionMode
    hasSelectionDoneButton: wrapper.inSelectionMode
    hasCancelSelectionButton: wrapper.inSelectionMode && wrapper.navbarHasCancelSelectionButton

    titleText: (!wrapper.inSelectionMode ? wrapper.navbarTitle : "")

    onReturnButtonPressed: wrapper.returnButtonPressed()
    onStateButtonPressed: wrapper.stateButtonPressed()
    onSelectionDoneButtonPressed: wrapper.selectionDoneButtonPressed()
    onCancelSelectionButtonPressed: wrapper.cancelSelectionButtonPressed()
  }

  GalleryStandardToolbar {
    id: toolbar

    anchors.bottom: parent.bottom

    background: (!wrapper.inSelectionMode ? "white" : "lightBlue")
    isTranslucent: wrapper.toolbarsAreTranslucent
    
    hasMainIconSet: (wrapper.inSelectionMode ? wrapper.toolbarHasMainIconsWhenSelecting : true)
    hasFullIconSet: wrapper.inSelectionMode || wrapper.toolbarHasFullIconSet
    useContrastOnWhiteColorScheme: wrapper.inSelectionMode

    onPageIndicatorPageSelected: wrapper.pageIndicatorPageSelected(pageNumber)

    onAlbumOperationsButtonPressed: {
      if (albumPicker.state == "hidden")
        wrapper.active = true;

      albumPicker.flipVisibility()
    }

    onMoreOperationsButtonPressed: wrapper.moreOperationsButtonPressed()

    onShareOperationsButtonPressed: wrapper.shareOperationsButtonPressed()
  }

  Timer {
    id: autoHideTimer

    function startAutoHide() {
      if (interval > 0)
        restart();
    }

    interval: 3000
    running: false

    onTriggered: chrome.state = "hidden"
  }
}

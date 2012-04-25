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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import "../Capetown"

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

  property bool navbarHasStateButton: false
  property bool navbarHasCancelSelectionButton: false

  property bool toolbarHasFullIconSet: true
  property bool toolbarHasMainIconsWhenSelecting: true

  property bool inSelectionMode: false
  property alias hasSelectionOperationsButton: toolbar.hasSelectionOperationsButton
  property bool hasSelectionNavbar: true

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
  property alias toolbarHasAlbumOperationsButton: toolbar.hasAlbumOperationsButton
  property alias toolbarPageIndicatorAlbum: toolbar.pageIndicatorAlbum
  signal pageIndicatorPageSelected(int page)
  signal moreOperationsButtonPressed()
  signal shareOperationsButtonPressed()
  signal albumOperationsButtonPressed()
  signal trashOperationButtonPressed()
  signal selectionOperationsButtonPressed(variant button)

  // Pass-throughs from the left/right nav buttons.
  signal leftNavigationButtonPressed()
  signal rightNavigationButtonPressed()

  // private properties -- don't touch these from outside this component
  property bool popupActive: false

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

    this.popupActive = target.visible;
  }

  function hideAllPopups() {
    for (var i = 0; i < popups.length; i++)
      popups[i].state = "hidden";

    this.popupActive = false;
  }

  onVisibleChanged: {
    wrapper.hidePopups();

    if (visible)
      autoHideTimer.startAutoHide();
  }

  onPopupActiveChanged: {
    if (popupActive)
      autoHideTimer.stop();
    else
      autoHideTimer.startAutoHide();
  }

  function cancelActivity() {
    this.hideAllPopups();
  }

  MouseArea {
    id: cancelArea

    anchors.fill: parent
    z:16

    visible: chrome.popupActive

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

  GalleryStandardNavbar {
    id: navbar

    property bool inSelectionMode: wrapper.inSelectionMode && wrapper.hasSelectionNavbar

    anchors.top: parent.top

    background: (!inSelectionMode ? "white" : "lightBlue")
    isTranslucent: (!inSelectionMode ? wrapper.toolbarsAreTranslucent : false)

    hasReturnButton: !inSelectionMode
    hasStateButton: wrapper.navbarHasStateButton && !inSelectionMode
    hasSelectionDoneButton: inSelectionMode
    hasCancelSelectionButton: inSelectionMode && wrapper.navbarHasCancelSelectionButton

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

    onPageIndicatorPageSelected: wrapper.pageIndicatorPageSelected(page)

    onAlbumOperationsButtonPressed: wrapper.albumOperationsButtonPressed()
    onMoreOperationsButtonPressed: wrapper.moreOperationsButtonPressed()
    onShareOperationsButtonPressed: wrapper.shareOperationsButtonPressed()
    onSelectionOperationsButtonPressed: wrapper.selectionOperationsButtonPressed(button)
    onTrashOperationButtonPressed: wrapper.trashOperationButtonPressed()
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

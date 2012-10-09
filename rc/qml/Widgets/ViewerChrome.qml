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

import QtQuick 2.0
import Gallery 1.0
import "../../Capetown"
import "../../js/Gallery.js" as Gallery

// The ViewerChrome object wraps toolbars and other chrome elements into one
// easy to use, standardized object.  You'll probably want to fill the screen
// with it, as it places its chrome children at its edges.
Item {
  id: wrapper

  property alias autoHideWait: autoHideTimer.interval // 0 to disable auto-hide.

  property variant popups: [ ]

  property bool hasLeftNavigationButton: false
  property bool hasRightNavigationButton: false

  property bool toolbarsAreTextured: true
  property bool toolbarsAreTranslucent: true
  property bool toolbarsAreDark: false

  property bool navbarHasStateButton: false
  property bool navbarHasCancelSelectionButton: false

  property bool toolbarHasFullIconSet: true
  property bool toolbarHasMainIconsWhenSelecting: true

  property bool inSelectionMode: false
  property alias hasSelectionOperationsButton: toolbar.hasSelectionOperationsButton
  
  property alias pagesPerSpread: toolbar.albumPagesPerSpread
  property alias viewingPage: toolbar.albumViewingPage

  // signals sent from the entire chrome ensemble
  signal hidePopups();

  // Pass-throughs from the navbar.
  property alias navbarHeight: navbar.height
  property alias navbarSelectedStateButtonIconFilename: navbar.selectedStateButtonIconFilename
  property alias navbarDeselectedStateButtonIconFilename: navbar.deselectedStateButtonIconFilename
  property alias navbarSelectionDoneButtonText: navbar.selectionDoneButtonText
  signal returnButtonPressed()
  signal stateButtonPressed()
  signal selectionDoneButtonPressed()
  signal cancelSelectionButtonPressed()

  // Pass-throughs from the toolbar.
  property alias toolbarHeight: toolbar.height
  property alias toolbarHasPageIndicator: toolbar.hasPageIndicator
  property alias toolbarHasAlbumOperationsButton: toolbar.hasAlbumOperationsButton
  property alias toolbarPageIndicatorAlbum: toolbar.pageIndicatorAlbum
  property alias toolbarHasEditOperationsButton: toolbar.hasEditOperationsButton
  signal pageIndicatorPageSelected(int page)
  signal moreOperationsButtonPressed()
  signal shareOperationsButtonPressed()
  signal albumOperationsButtonPressed()
  signal trashOperationButtonPressed()
  signal selectionOperationsButtonPressed(variant button)
  signal editOperationsButtonPressed()

  // Pass-throughs from the left/right nav buttons.
  signal leftNavigationButtonPressed()
  signal rightNavigationButtonPressed()

  // internal
  property bool popupActive: false
  property int fadeDuration
  property int fadeEasing

  visible: false
  state: "hidden"

  states: [
    State { name: "shown"; },
    State { name: "hidden"; }
  ]

  transitions: [
    Transition { from: "shown"; to: "hidden";
      FadeOutAnimation { target: wrapper; duration: fadeDuration;
          easingType: fadeEasing; }
    },
    Transition { from: "hidden"; to: "shown";
      FadeInAnimation { target: wrapper; duration: fadeDuration;
          easingType: fadeEasing; }
    }
  ]

  function flipVisibility(fromUserAction) {
    setFadeParams(fromUserAction);
    state = (state == "shown" ? "hidden" : "shown");
  }

  function resetVisibility(visibility) {
    state = ""; // To prevent animation.
    visible = visibility;
    state = (visibility ? "shown" : "hidden");
  }

  function show(fromUserAction) {
    setFadeParams(fromUserAction);
    state = "shown";
  }

  function hide(fromUserAction) {
    setFadeParams(fromUserAction);
    state = "hidden";
  }

  function cyclePopup(target) {
    if (!target.visible)
      hideAllPopups();

    target.flipVisibility();
    popupActive = target.visible;
  }

  function hideAllPopups() {
    for (var i = 0; i < popups.length; i++)
      popups[i].state = "hidden";

    popupActive = false;
  }

  // internal
  function setFadeParams(fromUserAction) {
    fadeDuration = (fromUserAction ? Gallery.SNAP_DURATION : Gallery.SLOW_DURATION);
    fadeEasing = (fromUserAction ? Easing.InQuint : Easing.InOutQuint);
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
    hideAllPopups();
  }

  MouseArea {
    id: cancelArea

    anchors.fill: parent
    z:16

    visible: chrome.popupActive

    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: chrome.cancelActivity()
  }

  ViewerNavigationButton {
    id: leftNavButton

    is_forward: false

    visible: wrapper.hasLeftNavigationButton && !wrapper.inSelectionMode

    anchors.leftMargin: gu(1.5)
    anchors.left: parent.left
    anchors.bottom: toolbar.top

    onPressed: {
      autoHideTimer.startAutoHide();
      leftNavigationButtonPressed();
    }
  }

  ViewerNavigationButton {
    id: rightNavButton

    is_forward: true

    visible: wrapper.hasRightNavigationButton && !wrapper.inSelectionMode

    anchors.rightMargin: gu(1.5)
    anchors.right: parent.right
    anchors.bottom: toolbar.top

    onPressed: {
      autoHideTimer.startAutoHide();
      rightNavigationButtonPressed();
    }
  }

  GalleryStandardNavbar {
    id: navbar

    anchors.top: parent.top

    isTextured: wrapper.toolbarsAreTextured
    isTranslucent: (!wrapper.inSelectionMode
                    ? wrapper.toolbarsAreTranslucent : false)
    isDark: wrapper.toolbarsAreDark

    hasReturnButton: !wrapper.inSelectionMode
    hasStateButton: wrapper.navbarHasStateButton && !wrapper.inSelectionMode
    hasSelectionDoneButton: wrapper.inSelectionMode
    hasCancelSelectionButton: wrapper.inSelectionMode &&
                              wrapper.navbarHasCancelSelectionButton

    onReturnButtonPressed: wrapper.returnButtonPressed()
    onStateButtonPressed: wrapper.stateButtonPressed()
    onSelectionDoneButtonPressed: wrapper.selectionDoneButtonPressed()
    onCancelSelectionButtonPressed: wrapper.cancelSelectionButtonPressed()
  }

  GalleryStandardToolbar {
    id: toolbar

    anchors.bottom: parent.bottom

    isTextured: wrapper.toolbarsAreTextured
    isTranslucent: wrapper.toolbarsAreTranslucent
    isDark: wrapper.toolbarsAreDark

    hasMainIconSet: (wrapper.inSelectionMode ? wrapper.toolbarHasMainIconsWhenSelecting : true)
    hasFullIconSet: wrapper.inSelectionMode || wrapper.toolbarHasFullIconSet

    onPageIndicatorPageSelected: wrapper.pageIndicatorPageSelected(page)

    onAlbumOperationsButtonPressed: wrapper.albumOperationsButtonPressed()
    onMoreOperationsButtonPressed: wrapper.moreOperationsButtonPressed()
    onShareOperationsButtonPressed: wrapper.shareOperationsButtonPressed()
    onSelectionOperationsButtonPressed: wrapper.selectionOperationsButtonPressed(button)
    onTrashOperationButtonPressed: wrapper.trashOperationButtonPressed()
    onEditOperationsButtonPressed: wrapper.editOperationsButtonPressed()
  }

  Timer {
    id: autoHideTimer

    function startAutoHide() {
      if (interval > 0)
        restart();
    }

    interval: 3000
    running: false

    onTriggered: chrome.hide(false)
  }
}

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

Item {
  id: wrapper

  property int fadeDuration: 200
  property bool hasPopupMenu: true
  property alias autoHideWait: autoHideTimer.interval // 0 to disable auto-hide.

  // Pass-throughs from the toolbar.
  property alias toolbarHeight: toolbar.height
  property alias toolbarIsTranslucent: toolbar.isTranslucent
  property alias toolbarOpacity: toolbar.opacity
  property alias toolbarBackground: toolbar.background
  property alias toolbarHasFullIconSet: toolbar.hasFullIconSet
  property alias toolbarHasReturnButton: toolbar.hasReturnButton
  property alias toolbarUseContrastOnWhiteColorScheme: toolbar.useContrastOnWhiteColorScheme
  signal trashOperationButtonPressed()
  signal shareOperationsButtonPressed()
  signal returnButtonPressed()

  // Pass-throughs from the left/right nav buttons.
  property alias leftNavigationButtonVisible: leftNavButton.visible
  property alias rightNavigationButtonVisible: rightNavButton.visible
  signal leftNavigationButtonPressed()
  signal rightNavigationButtonPressed()

  // Pass-throughs from the album picker.
  property alias albumPickerDesignatedModel: albumPicker.designated_model
  signal albumPicked(variant album)
  signal newAlbumPicked()

  // Pass-throughs from the popup menu.
  property alias popupMenuItemTitle: menu.itemTitle
  signal menuItemChosen()

  // Read-only, please.
  property bool active: (albumPicker.visible || menu.visible)

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

  onVisibleChanged: {
    albumPicker.resetVisibility(false);
    menu.resetVisibility(false);
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
    albumPicker.state = "hidden";
    menu.state = "hidden";
  }

  MouseArea {
    id: cancelArea

    anchors.fill: parent

    visible: chrome.active

    onClicked: chrome.cancelActivity()
  }

  ViewerNavigationButton {
    id: leftNavButton

    is_forward: false

    x: 12
    y: 2 * parent.height / 3

    onPressed: {
      autoHideTimer.startAutoHide();
      leftNavigationButtonPressed();
    }
  }

  ViewerNavigationButton {
    id: rightNavButton

    is_forward: true

    x: parent.width - width - 12
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

  PlaceholderPopupMenu {
    id: menu

    anchors.bottom: toolbar.top
    x: toolbar.moreOperationsPopupX - width

    onItemChosen: {
      menuItemChosen();
      state = "hidden";
    }
  }

  GalleryStandardToolbar {
    id: toolbar

    anchors.bottom: parent.bottom

    onAlbumOperationsButtonPressed: albumPicker.flipVisibility()

    onMoreOperationsButtonPressed: {
      if (hasPopupMenu)
        menu.flipVisibility();
    }

    onTrashOperationButtonPressed: wrapper.trashOperationButtonPressed()
    onShareOperationsButtonPressed: wrapper.shareOperationsButtonPressed()
    onReturnButtonPressed: wrapper.returnButtonPressed()
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

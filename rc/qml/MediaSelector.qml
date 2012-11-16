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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import "../Capetown"
import "Components"
import "Utility"
import "Widgets"

Item {
  id: mediaSelector
  
  signal cancelRequested()
  signal doneRequested(variant model)
  signal mediaCheckerboardHidden(int newScrollPos)

  property variant album
  
  // Read-only.
  property bool animationRunning: slider.animationRunning
  
  function show() {
    slider.slideIn();
  }

  function hide() {
    slider.slideOut();
  }

  function setCheckerboardScrollPos(newScrollPos) {
      mediaCheckerboard.setScrollPos(newScrollPos);
  }
  
  SlidingPane {
    id: slider

    x: 0
    y: parent.height
    width: parent.width
    height: parent.height

    inX: 0
    inY: 0

    visible: (y < parent.height)

    EventCheckerboard {
      id: mediaCheckerboard
      objectName: "mediaCheckerboard"

      anchors.fill: parent

      topExtraGutter: chrome.navbarHeight + getDeviceSpecific("photoGridTopMargin")
      bottomExtraGutter: gu(0)
      leftExtraGutter: getDeviceSpecific("photoGridLeftMargin")
      rightExtraGutter: getDeviceSpecific("photoGridRightMargin")

      allowSelectionModeChange: false
      inSelectionMode: true
      disableMediaInAlbum: mediaSelector.album

      onHidden: {
        mediaCheckerboardHidden(currScrollPos)
      }
    }

    ViewerChrome {
      id: chrome

      anchors.fill: parent

      autoHideWait: 0

      navbarSelectionDoneButtonText: "Add to album"
      navbarSelectionDoneButtonWidth: gu(18)
      navbarHasCancelSelectionButton: true

      toolbarHasMainIconsWhenSelecting: false

      inSelectionMode: true
      state: "shown"
      visible: true

      hasSelectionOperationsButton: true
      onSelectionOperationsButtonPressed: cyclePopup(selectionMenu);

      onSelectionDoneButtonPressed: {
        doneRequested(mediaCheckerboard.model);

        mediaCheckerboard.unselectAll();
      }

      onCancelSelectionButtonPressed: {
        mediaCheckerboard.unselectAll();
        cancelRequested();
      }

      popups: [selectionMenu]

      SelectionMenu {
        id: selectionMenu

        checkerboard: mediaCheckerboard

        onPopupInteractionCompleted: chrome.hideAllPopups()
      }
    }
  }
}

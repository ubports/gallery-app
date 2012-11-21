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

  property variant album
  
  // Read-only.
  property bool animationRunning: slider.animationRunning
  
  function show() {
    slider.slideIn();
  }

  function hide() {
    slider.slideOut();
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

    OrganicEventView {
      id: photos

      anchors.fill: parent
      anchors.topMargin: chrome.navbarHeight
      visible: true

      selection: SelectionState {
        inSelectionMode: true
        allowSelectionModeChange: false
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
        doneRequested(photos.selection.model);
        photos.selection.unselectAll();
      }

      onCancelSelectionButtonPressed: {
        photos.selection.unselectAll();
        cancelRequested();
      }

      popups: [selectionMenu]

      SelectionMenu {
        id: selectionMenu

        selection: photos.selection

        onPopupInteractionCompleted: chrome.hideAllPopups()
      }
    }
  }
}

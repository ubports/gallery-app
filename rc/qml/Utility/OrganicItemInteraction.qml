/*
 * Copyright (C) 2012 Canonical Ltd
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

import QtQuick 2.0
import Ubuntu.Components 0.1

// The user interaction and selection handling for items in the
// OrganicMediaList.
Item {
  id: organicItemInteraction

  /*!
  */
  signal pressed()

  /*!
  */
  property var selectionItem
  /*!
  */
  property SelectionState selection

  // readonly
  property bool isSelected: selection.isSelected(selectionItem)

  anchors.fill: parent

  // FIXME: this is temporary and should be replaced with something real.
  Image {
    id: selectionTick

    anchors.right: parent.right
    anchors.top: parent.top
    width: units.gu(5)
    height: units.gu(5)

    visible: isSelected

    source: "img/photo-preview-selected-overlay.png"
  }

  MouseArea {
    anchors.fill: parent

    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onPressAndHold: selection.toggleSelection(selectionItem)
    onClicked: {
      if (mouse.button == Qt.RightButton || selection.inSelectionMode)
        selection.toggleSelection(selectionItem);
      else
        organicItemInteraction.pressed();
    }
  }
}

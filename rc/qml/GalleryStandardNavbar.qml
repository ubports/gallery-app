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

Toolbar {
  id: wrapper

  property bool hasReturnButton: true
  property bool hasStateButton: false
  property url stateButtonIconFilename
  property bool hasSelectionDoneButton: false
  property alias selectionDoneButtonTitle: selectionDoneButton.title
  property bool hasCancelSelectionButton: false
  property alias titleText: title.text
  property alias titleColor: title.color

  signal returnButtonPressed()
  signal stateButtonPressed()
  signal selectionDoneButtonPressed()
  signal cancelSelectionButtonPressed()

  background: "white"

  Row {
    id: leftIconGroup

    spacing: gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: gu(2)

    GallerySecondaryPushButton {
      id: cancelSelectionButton

      title: "cancel"

      visible: wrapper.hasCancelSelectionButton

      onPressed: wrapper.cancelSelectionButtonPressed()
    }

    ToolbarIconButton {
      visible: wrapper.hasReturnButton

      selectedIconFilename: "../img/return-arrow.png"
      deselectedIconFilename: selectedIconFilename
      
      onPressed: wrapper.returnButtonPressed()
    }

    ToolbarIconButton {
      visible: wrapper.hasStateButton

      selectedIconFilename: stateButtonIconFilename
      deselectedIconFilename: stateButtonIconFilename
      
      onPressed: wrapper.stateButtonPressed()
    }
  }

  Text {
    id: title

    anchors.centerIn: parent

    font.pixelSize: gu(2)
    color: "#bfb7ae"
  }

  Row {
    id: rightIconGroup

    spacing: gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: gu(2)

    GalleryPrimaryPushButton {
      id: selectionDoneButton

      title: "done"

      visible: wrapper.hasSelectionDoneButton

      onPressed: wrapper.selectionDoneButtonPressed()
    }
  }
}

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
import "../../Capetown/Widgets"

Toolbar {
  id: wrapper

  property bool hasReturnButton: true
  property bool hasStateButton: false
  property alias selectedStateButtonIconFilename: stateButton.selectedIconFilename
  property alias deselectedStateButtonIconFilename: stateButton.deselectedIconFilename
  property bool hasSelectionDoneButton: false
  property alias selectionDoneButtonText: selectionDoneButton.text
  property bool hasCancelSelectionButton: false

  signal returnButtonPressed()
  signal stateButtonPressed()
  signal selectionDoneButtonPressed()
  signal cancelSelectionButtonPressed()

  Row {
    id: leftIconGroup

    spacing: gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: gu(2)

    ToolbarTextButton {
      text: "Cancel"

      visible: wrapper.hasCancelSelectionButton

      onPressed: wrapper.cancelSelectionButtonPressed()
    }

    ReturnToolbarButton {
      visible: wrapper.hasReturnButton
      isWhite: wrapper.isDark

      onPressed: wrapper.returnButtonPressed()
    }

    ToolbarIconButton {
      id: stateButton

      visible: wrapper.hasStateButton

      onPressed: wrapper.stateButtonPressed()
    }
  }

  Row {
    id: rightIconGroup

    spacing: gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: gu(2)

    ToolbarTextButton {
      id: selectionDoneButton

      text: "Done"

      visible: wrapper.hasSelectionDoneButton

      onPressed: wrapper.selectionDoneButtonPressed()
    }
  }
}

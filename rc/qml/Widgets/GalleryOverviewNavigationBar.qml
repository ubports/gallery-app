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
 * Lucas Beeler <lucas@yorba.org>
 */
 
import QtQuick 2.0
import Ubuntu.Components 0.1
import "../../Capetown/Widgets"

Toolbar {
  property alias addCreateOperationButtonVisible: addCreateOperationNavbarButton.visible
  signal addCreateOperationButtonPressed()

  AddCreateOperationNavbarButton {
    id: addCreateOperationNavbarButton

    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: units.gu(2)

    onClicked: parent.addCreateOperationButtonPressed()
  }

  ToolbarIconButton {
    objectName: "toolbarCameraIcon"
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: units.gu(2)

    selectedIconFilename: "img/icon-camera-active.png"
    deselectedIconFilename: "img/icon-camera-inactive.png"

    onPressAndHold: Qt.quit()
  }
}

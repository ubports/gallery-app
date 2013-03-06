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

import QtQuick 2.0
import Ubuntu.Components 0.1
import "../../Capetown/Widgets"

/*!
*/
Toolbar {
    id: wrapper

    /*!
    */
    property bool hasReturnButton: true
    /*!
    */
    property bool hasStateButton: false
    /*!
    */
    property alias selectedStateButtonIconFilename: stateButton.selectedIconFilename
    /*!
    */
    property alias deselectedStateButtonIconFilename: stateButton.deselectedIconFilename
    /*!
    */
    property bool hasSelectionDoneButton: false
    /*!
    */
    property alias selectionDoneButtonText: selectionDoneButton.text
    /*!
    */
    property alias selectionDoneButtonWidth: selectionDoneButton.width
    /*!
    */
    property bool hasCancelSelectionButton: false

    /*!
    */
    signal returnButtonPressed()
    /*!
    */
    signal stateButtonPressed()
    /*!
    */
    signal selectionDoneButtonPressed()
    /*!
    */
    signal cancelSelectionButtonPressed()

    Row {
        id: leftIconGroup

        spacing: units.gu(2)
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: units.gu(2)

        Button {
            height: units.gu(4)
            width: units.gu(15)

            text: "Cancel"

            visible: wrapper.hasCancelSelectionButton

            onClicked: wrapper.cancelSelectionButtonPressed()
        }

        ReturnToolbarButton {
            visible: wrapper.hasReturnButton
            isWhite: wrapper.isDark

            onClicked: wrapper.returnButtonPressed()
        }

        ToolbarIconButton {
            id: stateButton

            visible: wrapper.hasStateButton

            onClicked: wrapper.stateButtonPressed()
        }
    }

    Row {
        id: rightIconGroup

        spacing: units.gu(2)
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: units.gu(2)

        Button {
            id: selectionDoneButton
            height: units.gu(4)
            width: units.gu(15)

            text: "Done"

            visible: wrapper.hasSelectionDoneButton

            onClicked: wrapper.selectionDoneButtonPressed()
        }
    }
}

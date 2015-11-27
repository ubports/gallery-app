/*
 * Copyright (C) 2012-2015 Canonical Ltd
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import "../Utility"

// The user interaction and selection handling for items in the
// OrganicMediaList.
Item {
    id: organicItemInteraction

    /*!
    */
    signal pressed()
    signal selected()

    /*!
    */
    property var selectionItem
    /*!
    */
    property SelectionState selection

    // readonly
    property bool isSelected: selection.isSelected(selectionItem)

    property bool isEventHeader: false

    anchors.fill: parent

    function toggleSelection() {
        selection.toggleSelection(selectionItem);
        if (isSelected)
            organicItemInteraction.selected();
    }

    Rectangle {
        id: selectionRectangle
        objectName: "selectionCheckbox"
        anchors {
            top: parent.top
            right: parent.right
            topMargin: units.gu(0.5)
            rightMargin: units.gu(0.5)
        }
        width: units.gu(4)
        height: width 
        color: isSelected ? UbuntuColors.orange : UbuntuColors.coolGrey
        radius: 10
        opacity: isSelected ? 0.8 : 0.6
        visible: selection.inSelectionMode && !isEventHeader

        Icon {
            anchors.centerIn: parent
            width: parent.width * 0.8
            height: width
            name: "ok"
            color: "white"
            visible: isSelected
        }
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressAndHold: toggleSelection()
        onClicked: {
            if (mouse.button == Qt.RightButton || isEventHeader)
                toggleSelection();
            else
                organicItemInteraction.pressed();
        }
    }

    MouseArea {
        anchors {
            top: parent.top
            right: parent.right
        }
        width: parent.width * 0.5
        height: parent.height * 0.5
        enabled: selectionRectangle.visible

        onClicked: {
            mouse.accepted = true;
            toggleSelection();
        }
    }
}

/*
 * Copyright (C) 2011-2012 Canonical Ltd
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

/*!
*/
PopupBox {
    id: popupActionChoiceDialog

    /*!
    */
    signal action0Requested()
    /*!
    */
    signal action1Requested()

    /*!
    */
    property alias action0Title: action0Button.text
    /*!
    */
    property alias action1Title: action1Button.text

    width: units.gu(40);
    height: units.gu(14) + originCueHeight;

    Button {
        id: action0Button

        color: "#f15c22"

        anchors.left: parent.left
        anchors.leftMargin: units.gu(2)
        anchors.right: parent.right
        anchors.rightMargin: units.gu(2)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: units.gu(7.5) + originCueHeight

        onClicked: {
            popupActionChoiceDialog.action0Requested();
            popupActionChoiceDialog.popupInteractionCompleted();
        }
    }

    Button {
        id: action1Button

        color: "#f15c22"

        anchors.left: parent.left
        anchors.leftMargin: units.gu(2)
        anchors.right: parent.right
        anchors.rightMargin: units.gu(2)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: units.gu(2) + originCueHeight

        onClicked: {
            popupActionChoiceDialog.action1Requested();
            popupActionChoiceDialog.popupInteractionCompleted();
        }
    }
}

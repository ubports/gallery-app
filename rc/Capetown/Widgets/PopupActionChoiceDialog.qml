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
import "../../UbuntuComponents"

PopupDialog {
  id: popupActionChoiceDialog

  signal action0Requested()
  signal action1Requested()
  signal cancelRequested()

  property alias action0Title: action0Button.text
  property alias action1Title: action1Button.text
  property alias cancelTitle: cancelButton.text

  width: gu(40);
  height: gu(32) + originCueHeight;

  Button {
    id: action0Button

    color: "#f15c22"

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(13) + originCueHeight

    onClicked: {
      popupActionChoiceDialog.action0Requested();
      popupActionChoiceDialog.popupInteractionCompleted();
    }
  }

  Button {
    id: action1Button

    color: "#f15c22"

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(7.5) + originCueHeight

    onClicked: {
      popupActionChoiceDialog.action1Requested();
      popupActionChoiceDialog.popupInteractionCompleted();
    }
  }

  Button {
    id: cancelButton

    text: "Cancel"
    color: "#818285"

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(2) + originCueHeight

    onClicked: {
      popupActionChoiceDialog.cancelRequested();
      popupActionChoiceDialog.popupInteractionCompleted();
    }
  }
}

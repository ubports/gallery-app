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

import QtQuick 1.1
import "GalleryUtility.js" as GalleryUtility
import "../Capetown"

PopupDialog {
  id: popupActionChoiceDialog

  signal action0Requested()
  signal action1Requested()

  property alias action0Title: action0Button.title
  property alias action1Title: action1Button.title
  property alias cancelTitle: cancelButton.title

  width: gu(40);
  height: gu(32) + originCueHeight;

  PopupDialogButton {
    id: action0Button

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(13) + originCueHeight

    onPressed: {
      popupActionChoiceDialog.action0Requested();
      popupActionChoiceDialog.popupInteractionCompleted();
    }
  }

  PopupDialogButton {
    id: action1Button

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(7.5) + originCueHeight

    onPressed: {
      popupActionChoiceDialog.action1Requested();
      popupActionChoiceDialog.popupInteractionCompleted();
    }
  }

  PopupDialogButton {
    id: cancelButton

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(2) + originCueHeight

    isCancelButton: true

    onPressed: {
      popupActionChoiceDialog.action1Requested();
      popupActionChoiceDialog.popupInteractionCompleted();
    }
  }
}

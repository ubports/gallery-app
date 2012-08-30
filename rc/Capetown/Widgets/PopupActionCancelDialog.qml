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

PopupDialog {
  id: popupActionCancelDialog

  signal actionRequested()
  signal cancelRequested()

  property alias actionTitle: actionButton.title
  property alias cancelTitle: cancelButton.title

  width: gu(40);
  height: gu(26) + originCueHeight;

  PopupDialogButton {
    id: actionButton

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(7.5) + originCueHeight

    onPressed: {
      popupActionCancelDialog.actionRequested();
      popupActionCancelDialog.popupInteractionCompleted();
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
      popupActionCancelDialog.cancelRequested();
      popupActionCancelDialog.popupInteractionCompleted();
    }
  }
}

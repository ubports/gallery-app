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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1

PopupBox {
  id: popupActionDialog

  signal actionRequested()

  property alias actionTitle: actionButton.text

  width: gu(40);
  height: gu(9) + originCueHeight;

  Button {
    id: actionButton

    color: "#f15c22"

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(2) + originCueHeight

    onClicked: {
      popupActionDialog.actionRequested();
      popupActionDialog.popupInteractionCompleted();
    }
  }
}

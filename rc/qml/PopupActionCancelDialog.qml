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

PopupBox {
  id: popupActionCancelDialog

  signal confirmed()

  property alias explanatoryText: explanatoryText.text
  property alias actionButtonTitle: actionButtonText.text
  property alias cancelButtonTitle: cancelButtonText.text

  width: gu(40);
  height: gu(26) + originCueHeight;

  Text {
    id: explanatoryText

    anchors.top: parent.top
    anchors.topMargin: gu(2)
    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    height: gu(7)

    font.pixelSize: gu(2.25)

    color: "#818285"

    text: ""

    wrapMode: Text.WordWrap
  }

  Rectangle {
    id: actionButtonFrame

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(7.5) + originCueHeight
    height: gu(4)

    color: "#f15c22"

    radius: gu(1)

    Text {
      id: actionButtonText

      text: "OK"

      color: "white"

      anchors.fill: parent

      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
      anchors.fill: parent

      onClicked: {
        confirmed();
        popupInteractionCompleted();
      }
    }
  }

  Rectangle {
    id: cancelButtonFrame

    anchors.left: parent.left
    anchors.leftMargin: gu(2)
    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: gu(2) + originCueHeight
    height: gu(4)

    color: "#818285"

    radius: gu(1)

    Text {
      id: cancelButtonText

      text: "Cancel"

      color: "white"

      anchors.fill: parent

      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }
  }
}

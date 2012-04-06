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

import QtQuick 1.1
import Gallery 1.0
import "../Capetown"

PopupBox {
  id: popupAlbumPicker

  signal albumPicked(variant album);
  signal newAlbumRequested();
  signal popupInteractionCompleted();

  property alias albumModel: scroller.model

  width: gu(40)
  height: gu(80) + originCueHeight

  AddToNewAlbumButton {
    id: addToNewAlbumButton

    hostPopup: popupAlbumPicker

    onPressAnimationStarted: popupAlbumPicker.newAlbumRequested();

    onPressed: popupAlbumPicker.popupInteractionCompleted();
  }

  Timer {
    id: interactionCompletedTimer

    interval: 300

    onTriggered: popupAlbumPicker.popupInteractionCompleted()
  }

  Timer {
    id: addPhotoLingerTimer

    interval: 300

    onTriggered: {
      popupAlbumPicker.state = "hidden";
      interactionCompletedTimer.restart();
    }
  }

  Rectangle {
    width: parent.width - gu(0.5)
    height: gu(5)
    anchors.left: parent.left
    anchors.leftMargin: gu(0.25)
    anchors.rightMargin: gu(0.25)
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.bottomMargin: originCueHeight + gu(0.25)

    gradient: Gradient {
      GradientStop { position: 0.0; color: "#bcbdc0" }
      GradientStop { position: 1.0; color: "#6b6c6e" }
    }
  }

  ListView {
    id: scroller

    clip: true
    anchors.top: addToNewAlbumButton.bottom
    anchors.topMargin: gu(2)
    anchors.bottom: parent.bottom
    anchors.bottomMargin: originCueHeight + gu(2)
    anchors.horizontalCenter: parent.horizontalCenter
    width: gu(34)
    spacing: gu(2.75)

    footer: Rectangle {
      color: "transparent"

      height: scroller.spacing
      anchors.left: parent.left
      anchors.right: parent.right
    }

    model: AlbumCollectionModel {
    }

    delegate: AlbumPreviewComponent {
      album: model.album

      x: gu(2.75)
      clip: true

      MouseArea {
        anchors.fill: parent

        onClicked: {
          popupAlbumPicker.albumPicked(album);
          addPhotoLingerTimer.restart();
        }
      }
    }
  }

  Rectangle {
    id: overstroke

    width: parent.width
    height: parent.height - parent.originCueHeight

    color: "transparent"

    border.color: "#a7a9ac"
    border.width: gu(0.25)
  }
}

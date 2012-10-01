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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import "../../Capetown/Widgets"
import "../Components"
import "../../js/Gallery.js" as Gallery

PopupBox {
  id: popupAlbumPicker

  signal albumPicked(variant album);

  property alias albumModel: scroller.model

  width: gu(40)
  height: gu(80) + originCueHeight

  Timer {
    id: interactionCompletedTimer

    interval: Gallery.FAST_DURATION

    onTriggered: popupAlbumPicker.popupInteractionCompleted()
  }

  Timer {
    id: addPhotoLingerTimer

    interval: Gallery.FAST_DURATION

    onTriggered: {
      popupAlbumPicker.state = "hidden";
      interactionCompletedTimer.restart();
    }
  }

  GridView {
    id: scroller

    property int albumPreviewWidth: gu(14);
    property int albumPreviewHeight: gu(16.5);
    property int gutterWidth: gu(2)
    property int gutterHeight: gu(4)

    clip: true
    anchors.top: titleTextFrame.bottom
    anchors.bottom: parent.bottom
    anchors.bottomMargin: originCueHeight + gu(0.25)
    anchors.left: parent.left
    anchors.leftMargin: gu(4)
    anchors.right: parent.right

    cellWidth: scroller.albumPreviewWidth + scroller.gutterWidth
    cellHeight: scroller.albumPreviewHeight + scroller.gutterHeight

    header: Item {
      width: parent.width
      height: gu(2);
    }
    footer: Item {
      width: parent.width
      height: scroller.gutterHeight / 2
    }

    model: AlbumCollectionModel {
    }

    delegate: Item {
      width: scroller.cellWidth
      height: scroller.cellHeight

      AlbumPreviewComponent {
        album: model.album

        width: scroller.albumPreviewWidth
        height: scroller.albumPreviewHeight
        anchors.centerIn: parent

        states: [
          State { name: "unconfirmed";
            PropertyChanges { target: confirmCheck; opacity: 0.0 }
          },

          State { name: "confirmed";
            PropertyChanges { target: confirmCheck; opacity: 1.0 }
          }
        ]

        transitions: [
          Transition { from: "unconfirmed"; to: "confirmed";
            NumberAnimation { target: confirmCheck; property: "opacity";
              duration: addPhotoLingerTimer.interval;
              easing.type: Easing.InOutQuint }
          }
        ]

        state: "unconfirmed"

        Image {
          id: confirmCheck

          anchors.centerIn: parent;
          width: gu(7);
          height: gu(7);

          source: "img/confirm-check.png"
        }

        Timer {
          id: confirmStateResetTimer

          interval: addPhotoLingerTimer.interval +
            interactionCompletedTimer.interval

          onTriggered: parent.state = "unconfirmed"
        }

        MouseArea {
          anchors.fill: parent

          onClicked: {
            parent.state = "confirmed"
            popupAlbumPicker.albumPicked(album);
            addPhotoLingerTimer.restart();
            confirmStateResetTimer.restart();
          }
        }
      }
    }
  }

  Rectangle {
    id: titleTextFrame

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top

    height: gu(3)

    border.color: "white"
    border.width: gu(0.125);
    color: "transparent"

    Text {
      id: titleText

      anchors.fill: parent
      anchors.leftMargin: gu(0.5)
      anchors.topMargin: gu(0.5)

      color: "white"
      font.pixelSize: gu(2)
      font.italic: true
      font.weight: Font.Light

      text: "Add Photo to Album"
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

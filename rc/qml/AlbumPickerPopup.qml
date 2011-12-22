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

Rectangle {
  id: album_popup_wrapper
  objectName: "album_popup_wrapper"

  signal selected(int album_number);

  property variant designated_model

  width: 432
  height: 720
  z: 20

  color: "transparent"

  Rectangle {
    id: album_popup_body
    objectName: "album_popup_body"

    color: "#9d9d9d"

    x: 0
    y: 0
    width: parent.width
    height: parent.height - popup_arrow.height
    clip: true

    Rectangle {
      id: button_bar
      objectName: "button_bar"

      x: 0
      y: 0
      width: parent.width
      height: 40
      color: "white"

      Text {
        anchors.centerIn: parent

        text: "Add to album"

        color: "#008ecb"
      }

      SquareButton {
        title: "cancel"

        x: 4
        y: parent.height / 2 - height / 2

        onPressed: album_popup_wrapper.visible = false
      }

      SquareButton {
        title: "+ new"

        is_primary: true

        x: parent.width - width - 4
        y: parent.height / 2 - height / 2
      }
    }
  }

  ListView {
    id: album_preview_scoller
    objectName: "album_preview_scroller"

    x: 0
    y: button_bar.height + 6
    clip: true
    spacing: 22
    width: parent.width
    height: parent.height - button_bar.height - popup_arrow.height - 12

    model: album_popup_wrapper.designated_model

    delegate: AlbumPreviewDelegate {
      qmlRC: model.qml_rc
      albumName: model.album_name
      previewList: model.preview_list
      
      x: 22
      clip: true
      
      MouseArea {
        anchors.fill: parent
        
        onClicked: {
          album_popup_wrapper.visible = false;
          album_popup_wrapper.selected(object_number);
        }
      }
    }
  }

  Image {
    id: popup_arrow
    objectName: "popup_arrow"

    source: "../img/popup-arrow.png"

    width: 33
    height: 24
    x: parent.width - width
    y: parent.height - height
  }
}

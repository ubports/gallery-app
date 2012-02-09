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

Rectangle {
  id: album_popup_wrapper
  objectName: "album_popup_wrapper"

  signal selected(variant album);
  signal newAlbumRequested();

  property variant designated_model

  width: 432
  height: 720
  z: 20

  color: "transparent"

  states: [
    State { name: "shown";
      PropertyChanges { target: album_popup_wrapper; opacity: 1; } },

    State { name: "hidden";
      PropertyChanges { target: album_popup_wrapper; opacity: 0; } }
  ]

  transitions: [
    Transition { from: "*"; to: "*";
      NumberAnimation { properties: "opacity"; easing.type:
                        Easing.InQuad; duration: 200; } }
  ]

  state: "hidden"

  function flipVisibility() {
    if (this.state == "shown")
      this.state = "hidden";
    else
      this.state = "shown";
  }

  Rectangle {
    id: album_popup_body
    objectName: "album_popup_body"

    gradient: Gradient {
      GradientStop { position: 0.0; color: "#6b6c6e" }
      GradientStop { position: 0.1; color: "#bcbdc0" }
      GradientStop { position: 0.95; color: "#bcbdc0" }
      GradientStop { position: 1.0; color: "#6b6c6e" }
    }

    x: 0
    y: 0
    z: 1
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

      GallerySecondaryPushButton {
        title: "cancel"
        isCompact: true

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter

        onPressed: { album_popup_wrapper.state = "hidden"; }
      }

      GalleryPrimaryPushButton {
        title: "+ new"
        isCompact: true

        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter

        onPressed: { album_popup_wrapper.newAlbumRequested(); }
      }
    }

    ListView {
      id: album_preview_scoller
      objectName: "album_preview_scroller"

      z: 1
      clip: true
      anchors.top: button_bar.bottom
      anchors.bottom: parent.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      spacing: 22

      header: Rectangle {
        color: "transparent"

        height: 22;
        anchors.left: parent.left
        anchors.right: parent.right
      }

      footer: Rectangle {
        color: "transparent"

        height: 22;
        anchors.left: parent.left
        anchors.right: parent.right
      }

      model: AlbumCollectionModel {
      }

      delegate: AlbumPreviewComponent {
        album: model.album
        
        x: 22
        clip: true
        
        MouseArea {
          anchors.fill: parent
          
          onClicked: {
            album_popup_wrapper.selected(album);
            album_popup_wrapper.state = "hidden";
          }
        }
      }
    }
  }
  
  Rectangle {
    color: "transparent"
    border.color: "#a7a9ac"
    border.width: 2

    width: parent.width
    height: parent.height - popup_arrow.height
    z: 0
  }

  Image {
    id: popup_arrow
    objectName: "popup_arrow"

    source: "../img/popup-arrow.png"

    width: 39
    height: 25

    x: parent.width - width
    y: parent.height - height
  }
}

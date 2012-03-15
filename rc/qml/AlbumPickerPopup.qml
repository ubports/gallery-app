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

Rectangle {
  id: album_popup_wrapper
  objectName: "album_popup_wrapper"

  signal selected(variant album);
  signal newAlbumRequested();

  property variant designated_model

  width: gu(54)
  height: gu(90)
  z: 20

  color: "transparent"

  states: [
    State { name: "shown"; },
    State { name: "hidden"; }
  ]

  transitions: [
    Transition { from: "shown"; to: "hidden";
      FadeOutAnimation { target: album_popup_wrapper; }
    },
    Transition { from: "hidden"; to: "shown";
      FadeInAnimation { target: album_popup_wrapper; }
    }
  ]

  state: "hidden"
  visible: false

  function flipVisibility() {
      state = (state == "shown" ? "hidden" : "shown");
  }

  function resetVisibility(visibility) {
    state = ""; // To prevent animation.
    state = (visibility ? "shown" : "hidden");
    visible = visibility;
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
      height: gu(5)
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
        anchors.leftMargin: gu(0.5)
        anchors.verticalCenter: parent.verticalCenter

        onPressed: { album_popup_wrapper.state = "hidden"; }
      }

      GalleryPrimaryPushButton {
        title: "+ new"
        isCompact: true

        anchors.right: parent.right
        anchors.rightMargin: gu(0.5)
        anchors.verticalCenter: parent.verticalCenter

        onPressed: { album_popup_wrapper.newAlbumRequested(); }
      }
    }

    ListView {
      id: scroller
      
      z: 1
      clip: true
      anchors.top: button_bar.bottom
      anchors.bottom: parent.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      spacing: gu(2.75)

      header: Rectangle {
        color: "transparent"

        height: scroller.spacing
        anchors.left: parent.left
        anchors.right: parent.right
      }

      footer: Rectangle {
        color: "transparent"

        height: scroller.spacer
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
    border.width: gu(0.25)

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

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

Rectangle {
  id: popupMenu

  property alias model: menuItemList.model
  property int popupOriginX: parent.width -
    Math.floor(popupBox.originCueHeight / 2)
  property int popupOriginY: parent.height - gu(6) /* toolbar height */
  property int fadeDuration: 200

  state: "hidden"

  states: [
    State { name: "shown"; },
    State { name: "hidden"; }
  ]

  transitions: [
    Transition { from: "shown"; to: "hidden";
      FadeOutAnimation { target: popupMenu; duration: fadeDuration; }
    },
    Transition { from: "hidden"; to: "shown";
      FadeInAnimation { target: popupMenu; duration: fadeDuration; }
    }
  ]

  function flipVisibility() {
      state = (state == "shown" ? "hidden" : "shown");
  }

  width: gu(40)
  x: popupOriginX - width + Math.floor(popupBox.originCueHeight / 2)
  y: popupOriginY - height;
  z: 20

  color: "transparent"

  PopupBox {
    id: popupBox

    anchors.fill: parent
  }

  ListView {
    id: menuItemList

    x: popupBox.contentLeft
    y: popupBox.contentTop;
    width: gu(40)
    height: 1 // height is set dynamically once content height can be
              // computed
    clip: true;

    interactive: false

    delegate: MenuItem {
      title: model.title
      isSeparator: model.isSeparator
      hasBottomBorder: model.hasBottomBorder
    }

    onContentHeightChanged: {
      popupMenu.height = contentHeight + popupBox.originCueHeight;
      overstroke.height = popupMenu.height - popupBox.originCueHeight;
      popupMenu.y = popupMenu.popupOriginY - popupMenu.height;

      height = contentHeight;
    }

    MouseArea {
      id: mouseBlocker;

      anchors.fill: parent;

      onClicked: popupMenu.state = "hidden"
    }
  }

  Rectangle {
    id: overstroke

    width: parent.width
    height: parent.height - popupBox.originCueHeight

    color: "transparent"

    border.color: "#a7a9ac"
    border.width: 1
  }
}

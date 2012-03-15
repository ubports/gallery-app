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
import "../Capetown"

Rectangle {
  id: popupMenu

  property alias model: menuItemList.model
  property int popupOriginX: parent.width -
    Math.floor(popupBox.originCueWidth / 2)
  property int popupOriginY: parent.height - gu(6) /* toolbar height */
  property int fadeDuration: 300
  property bool ready: false

  signal actionInvoked(string name);
  signal popupInteractionCompleted();

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
  x: {
    var cueArrowOffset = Math.floor(popupBox.originCueWidth / 2)
    if (popupOriginX >= 0)
      return popupOriginX - width + cueArrowOffset;
    else
      return parent.width - width + popupOriginX + cueArrowOffset;
  }
  y: popupOriginY - height;
  z: 32

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
      title: (model.title) ? model.title : ""
      isSeparator: (model.isSeparator) ? model.isSeparator : false
      hasBottomBorder: (model.hasBottomBorder) ? model.hasBottomBorder : false
      iconFilename: (model.iconFilename) ? model.iconFilename : ""
      hasCueRectangle: (model.hasCueRectangle) ? model.hasCueRectangle : false
      action: (model.action) ? model.action : ""
      hostMenu: popupMenu

      onActionInvoked: {
        popupMenu.actionInvoked(name);
      }

      onPopupInteractionCompleted: {
        popupMenu.popupInteractionCompleted();
      }
    }

    onContentHeightChanged: {
      popupMenu.height = contentHeight + popupBox.originCueHeight;
      overstroke.height = popupMenu.height - popupBox.originCueHeight;
      popupMenu.y = popupMenu.popupOriginY - popupMenu.height;

      height = contentHeight;
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

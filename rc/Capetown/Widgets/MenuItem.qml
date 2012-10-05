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

import QtQuick 2.0

Rectangle {
  id: menuItem

  property bool isSeparator: false
  property alias title: menuItemCaption.text
  property bool hasBottomBorder: true
  property alias iconFilename: iconImage.source
  property alias hasCueRectangle: cueRectangle.visible
  property string action: ""
  property Item hostMenu

  signal actionInvoked(string name)
  signal popupInteractionCompleted()

  states: [
    State {
      name: "normal";

      PropertyChanges {
        target: menuItem;
        color: (!menuItem.isSeparator) ? "white" : "#bcbdc0";
      }

      PropertyChanges {
        target: menuItemCaption;
        color: (!menuItem.isSeparator) ? "#818285" : "white"
      }
    },

    State {
      name: "highlight";

      PropertyChanges {
        target: menuItem;
        color: (!menuItem.isSeparator) ? "#bcbdc0" : "white"
      }

      PropertyChanges {
        target: menuItemCaption;
        color: "white" }
    }]

  state: "normal"
  
  function dispatchAction() {
    acknowledgeItemPressTimer.restart();
    clearHighlightTimer.restart();
    actionDispatchTimer.restart();
  }

  Timer {
    id: clearHighlightTimer;

    interval: 650

    onTriggered: {
      menuItem.state = "normal"
    }
  }

  Timer {
    id: actionDispatchTimer

    interval: clearHighlightTimer.interval

    onTriggered: {
      if (menuItem.action != "")
        menuItem.actionInvoked(menuItem.action);
    }
  }

  Timer {
    id: acknowledgeItemPressTimer

    interval: 300

    onTriggered: popupInteractionCompleted()
  }

  height: {
    if (isSeparator)
      return (title != "") ? gu(3) : gu(1)
    else
      return gu(6)
  }

  width: gu(40)

  clip: true;

  Image {
    id: iconImage

    visible: (iconFilename != "") ? true : false

    cache: true

    width: 36
    height: 36
    anchors.left: parent.left
    anchors.verticalCenter: parent.verticalCenter
    anchors.leftMargin: gu(1)
  }

  Rectangle {
    id: cueRectangle

    width: gu(3);
    height: gu(3);

    anchors.right: parent.right
    anchors.rightMargin: gu(1.5)
    anchors.verticalCenter: parent.verticalCenter

    color: "#bcbdc0"
    border.color: "#818285"
    border.width: 1
    radius: 4
  }

  Text {
    id: menuItemCaption

    anchors.left: (iconFilename != "") ? iconImage.right : parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.bottom: parent.bottom

    horizontalAlignment: (isSeparator || iconFilename != "") ? Text.AlignLeft :
      Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    font.family: "Ubuntu"
    font.weight: (isSeparator) ? Font.Bold : Font.Normal
    font.pixelSize: (isSeparator) ? gu(1.75) : gu(2)

    anchors.leftMargin: (isSeparator || iconFilename != "") ? gu(1) : 0

    visible: (text) ? true : false
  }

  Rectangle {
    width: parent.width
    height: 1
    anchors.bottom: parent.bottom

    visible: !menuItem.isSeparator && menuItem.hasBottomBorder

    color: "#bcbdc0"
  }

  MouseArea {
    anchors.fill: parent;

    onPressed: {
      if (!menuItem.isSeparator)
        menuItem.state = "highlight"

      dispatchAction();
    }
  }
}


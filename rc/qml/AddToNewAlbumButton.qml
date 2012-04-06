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
  id: addToNewAlbumButton

  property Item hostPopup

  signal pressed();
  signal pressAnimationStarted();

  states: [
    State {
      name: "normal";
      PropertyChanges {
        target: addToNewAlbumButton;
        color: "#e1e9f6";
      }

      PropertyChanges {
        target: titleText;
        color: "#818285"
      }

      PropertyChanges {
        target: plusCueImage;
        source: "../img/add-to-new-album-unselected.png";
      }
    },

    State {
      name: "highlight";

      PropertyChanges {
        target: addToNewAlbumButton;
        color: "#95b5df"
      }

      PropertyChanges {
        target: titleText;
        color: "white" }

      PropertyChanges {
        target: plusCueImage;
        source: "../img/add-to-new-album-selected.png";
      }
    }]

  function notifyPressed() {
    state = "highlight"
    pressAnimationStarted();
    acknowledgeItemPressTimer.restart();
    clearHighlightTimer.restart();
    notificationDispatchTimer.restart();
  }

  Timer {
    id: clearHighlightTimer;

    interval: 650

    onTriggered: addToNewAlbumButton.state = "normal"
  }

  Timer {
    id: notificationDispatchTimer;

    interval: clearHighlightTimer.interval

    onTriggered: addToNewAlbumButton.pressed();
  }

  Timer {
    id: acknowledgeItemPressTimer

    interval: 300

    onTriggered: {
      chrome.cyclePopup(hostPopup);
      chrome.popupActive = false
    }
  }

  state: "normal"

  width: gu(34)
  height: gu(6)
  y: gu(3)
  anchors.horizontalCenter: parent.horizontalCenter

  border.color: "#95b5df"
  border.width: gu(0.5)

  Text {
    id: titleText

    anchors.centerIn: parent
    text: "Add to new album"

    font.pixelSize: gu(2.25)

  }

  Image {
    id: plusCueImage
    source: "../img/add-to-new-album-unselected.png";

    width: 32
    height: 32
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: gu(1)
  }

  MouseArea {
    anchors.fill: parent;

    onPressed: parent.notifyPressed()
  }
}

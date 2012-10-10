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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org
 */

import QtQuick 2.0
import "../js/Gallery.js" as Gallery

Item {
  id: application
  
  // readonly
  property bool isPortrait: (height > width)

  // Converts a grid-unit into a pixel value.
  function gu(amt) {
    return Math.floor(GRIDUNIT * amt);
  }

  // gu(), but for font point sizes.
  function pointUnits(amt) {
    // 1.6 is a hacky fudge-factor for now.
    return Math.round(1.6 * amt);
  }

  function onLoaded() {
    mainScreenLoader.load();
    loadingScreen.visible = false;
  }
  
  width: gu(DEVICE_WIDTH)
  height: gu(DEVICE_HEIGHT)

  focus: true
  Keys.onPressed: {
    if (Gallery.isDesktop())
      return;

    if ((event.key == Qt.Key_L && isPortrait) ||
        (event.key == Qt.Key_P && !isPortrait)) {
      var temp = width;
      width = height;
      height = temp;
    }
  }
  
  LoadingScreen {
    id: loadingScreen

    anchors.fill: parent
  }

  Loader {
    id: mainScreenLoader

    function load() {
      sourceComponent = mainScreenComponent;
    }

    anchors.fill: parent

    Component {
      id: mainScreenComponent

      MainScreen {
        anchors.fill: parent
      }
    }
  }
}

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

Toolbar {
  id: wrapper

  property bool hasFullIconSet: true;
  property bool hasReturnButton: true;
  property bool useContrastOnWhiteColorScheme: false

  /* read only properties */
  property int albumOperationsPopupX: albumOperationsToolbarButton.x +
    iconGroup.x + 34;
  property int moreOperationsPopupX: moreOperationsToolbarButton.x +
    iconGroup.x + 34;

  signal albumOperationsButtonPressed()
  signal trashOperationButtonPressed()
  signal shareOperationsButtonPressed()
  signal moreOperationsButtonPressed()
  signal returnButtonPressed()

  background: "white"

  Image {
    source: "../img/return-arrow.png"

    x: gu(6)

    visible: wrapper.hasReturnButton

    MouseArea {
      anchors.fill: parent

      onClicked: wrapper.returnButtonPressed()
    }
  }

  Row {
    id: iconGroup

    spacing: gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: gu(2)

    TrashOperationToolbarButton {
      anchors.verticalCenter: parent.verticalCenter

      visible: hasFullIconSet
      opacity: 1.0

      onPressed: wrapper.trashOperationButtonPressed();
    }

    AlbumOperationsToolbarButton {
      id: albumOperationsToolbarButton

      useContrastOnWhiteColorScheme: wrapper.useContrastOnWhiteColorScheme

      anchors.verticalCenter: parent.verticalCenter

      visible: hasFullIconSet
      opacity: 1.0

      onPressed: wrapper.albumOperationsButtonPressed();
    }

    ShareOperationsToolbarButton {
      anchors.verticalCenter: parent.verticalCenter

      opacity: 1.0

      onPressed: wrapper.shareOperationsButtonPressed();
    }

    MoreOperationsToolbarButton {
      id: moreOperationsToolbarButton

      anchors.verticalCenter: parent.verticalCenter

      opacity: 1.0

      onPressed: wrapper.moreOperationsButtonPressed();
    }
  }
}

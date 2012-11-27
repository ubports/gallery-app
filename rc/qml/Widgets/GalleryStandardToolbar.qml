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

import QtQuick 2.0
import "../../Capetown/Widgets"

Toolbar {
  id: wrapper

  property bool hasPageIndicator: false
  property alias pageIndicatorAlbum: pageIndicator.album
  property bool hasMainIconSet: true
  property bool hasFullIconSet: true
  property bool hasAlbumOperationsButton: true
  property alias hasSelectionOperationsButton: selectionToolbarButton.visible
  property alias hasEditOperationsButton: editToolbarButton.visible
  property alias albumPagesPerSpread: pageIndicator.pagesPerSpread
  property alias albumViewingPage: pageIndicator.viewingPage

  /* read only properties */
  property int albumOperationsPopupX: albumOperationsToolbarButton.x +
    iconGroup.x + 34;
  property int moreOperationsPopupX: moreOperationsToolbarButton.x +
    iconGroup.x + 34;

  signal pageIndicatorPageSelected(int page)
  signal albumOperationsButtonPressed()
  signal trashOperationButtonPressed()
  signal shareOperationsButtonPressed()
  signal moreOperationsButtonPressed()
  signal selectionOperationsButtonPressed(variant button)
  signal editOperationsButtonPressed()

  Row {
    spacing: units.gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.leftMargin: units.gu(2)

    SelectionOperationsToolbarButton {
      id: selectionToolbarButton

      visible: false

      onClicked: wrapper.selectionOperationsButtonPressed(selectionToolbarButton)
    }

    EditOperationsToolbarButton {
      id: editToolbarButton

      visible: false

      onClicked: wrapper.editOperationsButtonPressed()
    }
  }

  AlbumPageIndicator {
    id: pageIndicator
    
    anchors.centerIn: parent
    isPortrait: application.isPortrait
    
    color: "transparent"
    visible: wrapper.hasPageIndicator && indicatorDotCount > 1
    
    onSelected: wrapper.pageIndicatorPageSelected(page)
  }

  Row {
    id: iconGroup

    spacing: units.gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: units.gu(2)

    visible: hasMainIconSet

    TrashOperationToolbarButton {
      visible: hasFullIconSet
      isWhite: wrapper.isDark

      onClicked: wrapper.trashOperationButtonPressed();
    }

    AlbumOperationsToolbarButton {
      id: albumOperationsToolbarButton

      visible: hasFullIconSet && hasAlbumOperationsButton
      isWhite: wrapper.isDark

      onClicked: wrapper.albumOperationsButtonPressed();
    }

    ShareOperationsToolbarButton {
      isWhite: wrapper.isDark

      onClicked: wrapper.shareOperationsButtonPressed();
    }

    MoreOperationsToolbarButton {
      id: moreOperationsToolbarButton

      isWhite: wrapper.isDark

      onClicked: wrapper.moreOperationsButtonPressed();
    }
  }
}

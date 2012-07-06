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
import "../Capetown"

Toolbar {
  id: wrapper

  property bool hasPageIndicator: false
  property alias pageIndicatorAlbum: pageIndicator.album
  property bool hasMainIconSet: true
  property bool hasFullIconSet: true
  property bool hasAlbumOperationsButton: true
  property alias hasSelectionOperationsButton: selectionToolbarButton.visible

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

  SelectionOperationsToolbarButton {
    id: selectionToolbarButton
    
    anchors.left: parent.left
    anchors.verticalCenter: parent.verticalCenter
    anchors.leftMargin: gu(2)
    
    visible: false
    
    onPressed: wrapper.selectionOperationsButtonPressed(selectionToolbarButton)
  }
  
  AlbumPageIndicator {
    id: pageIndicator

    anchors.centerIn: parent

    color: "transparent"
    visible: (album) ? wrapper.hasPageIndicator && album.contentPageCount > 2 : false;

    onSelected: wrapper.pageIndicatorPageSelected(page)
  }

  Row {
    id: iconGroup

    spacing: gu(2)
    anchors.verticalCenter: parent.verticalCenter
    anchors.right: parent.right
    anchors.rightMargin: gu(2)

    visible: hasMainIconSet

    TrashOperationToolbarButton {
      visible: hasFullIconSet
      isWhite: wrapper.isDark

      onPressed: wrapper.trashOperationButtonPressed();
    }

    AlbumOperationsToolbarButton {
      id: albumOperationsToolbarButton

      visible: hasFullIconSet && hasAlbumOperationsButton
      isWhite: wrapper.isDark

      onPressed: wrapper.albumOperationsButtonPressed();
    }

    ShareOperationsToolbarButton {
      isWhite: wrapper.isDark

      onPressed: wrapper.shareOperationsButtonPressed();
    }

    MoreOperationsToolbarButton {
      id: moreOperationsToolbarButton

      isWhite: wrapper.isDark

      onPressed: wrapper.moreOperationsButtonPressed();
    }
  }
}

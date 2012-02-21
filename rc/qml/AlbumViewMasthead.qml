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
  id: mastheadBar
  objectName: "mastheadBar"

  signal viewModeChanged()
  signal deselectAllRequested()
  signal selectionInteractionCompleted()

  property bool isTemplateView: true
  property bool isSelectionInProgress: false
  property bool areItemsSelected: false
  property string albumName: ""


  Text {
    text: mastheadBar.albumName

    x: gu(16)
    y: (parent.height / 2) - (height / 2) + 1

    color: "#657CA9"
  }

  Image {
    source: (mastheadBar.isTemplateView) ? "../img/grid-view.png" :
      "../img/template-view.png";

    x: parent.width - width - gu(2.75)
    y: (parent.height / 2) - (height / 2) + 1

    visible: !mastheadBar.isSelectionInProgress

    MouseArea {
      anchors.fill: parent

      onClicked: {
        mastheadBar.isTemplateView = !mastheadBar.isTemplateView
        mastheadBar.viewModeChanged();
      }
    }
  }

  GalleryPrimaryPushButton {
    id: doneButton
    objectName: "doneButton"

    title: "done"

    anchors.right: parent.right
    anchors.rightMargin: gu(2)
    anchors.verticalCenter: parent.verticalCenter

    visible: mastheadBar.isSelectionInProgress

    onPressed: {
      mastheadBar.isSelectionInProgress = false;
      selectionInteractionCompleted();
    }
  }

  GallerySecondaryPushButton {
    title: "deselect"

    anchors.right: doneButton.left
    anchors.rightMargin: gu(2)
    anchors.verticalCenter: parent.verticalCenter

    visible: mastheadBar.isSelectionInProgress && mastheadBar.areItemsSelected

    onPressed: {
      mastheadBar.deselectAllRequested();
    }
  }
}

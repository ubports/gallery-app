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

NavToolbar {
  id: mastheadBar
  objectName: "mastheadBar"

  signal viewModeChanged()

  property bool isTemplateView: true
  property string albumName: ""


  Text {
    text: mastheadBar.albumName

    x: 128
    y: (parent.height / 2) - (height / 2) + 1

    color: "#657CA9"
  }

  Image {
    source: (mastheadBar.isTemplateView) ? "../img/grid-view.png" :
      "../img/template-view.png";

    x: parent.width - width - 22
    y: (parent.height / 2) - (height / 2) + 1

    MouseArea {
      anchors.fill: parent

      onClicked: {
        mastheadBar.isTemplateView = !mastheadBar.isTemplateView
        mastheadBar.viewModeChanged();
      }
    }
  }
}
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
 */

import QtQuick 1.1

Item {
  property alias mediaSource: photo.mediaSource
  property real leftGutter
  property real rightGutter
  property real topGutter
  property real bottomGutter
  property alias isPreview: photo.isPreview
  
  anchors.leftMargin: leftGutter
  anchors.rightMargin: rightGutter
  anchors.topMargin: topGutter
  anchors.bottomMargin: bottomGutter

  PhotoComponent {
    id: photo

    anchors.fill: parent

    ownerName: "FramePortrait"
    isCropped: true
  }
}

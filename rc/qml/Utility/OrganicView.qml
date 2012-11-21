/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0

// A ListView meant to hold OrganicMediaLists in some form or another.
Item {
  id: organicView

  signal mediaSourcePressed(var mediaSource, var thumbnailRect)

  property alias model: organicList.model
  property alias delegate: organicList.delegate
  property SelectionState selection

  // readonly
  // Some duplication from OrganicMediaList, to make certain things easier.
  property int organicMediaListMediaPerPattern: 6 // OrganicMediaList.mediaPerPattern
  property real organicMediaListPatternWidth: gu(72) // OrganicMediaList.patternWidth
  property real organicMediaListMargin: gu(3) // OrganicMediaList.margin

  Image {
    anchors.fill: parent

    source: "../../img/background-paper.png"
    fillMode: Image.Tile
  }

  ListView {
    id: organicList

    anchors.fill: parent
    clip: true

    // TODO: set cacheBuffer to some intelligent value so we cache the trays
    // more predictably.

    header: Item {
      width: parent.width
      height: organicMediaListMargin / 2
    }
    footer: Item {
      width: parent.width
      height: organicMediaListMargin / 2
    }
  }
}

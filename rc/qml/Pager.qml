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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1

// A Pager is a ListView of screen-sized items whose currentIndex tracks which page the user is
// currently on.
ListView {
  id: pager
  objectName: "pager"
  
  property int pageCacheSize: 4
  
  function pageForward() {
    incrementCurrentIndex();
    positionViewAtIndex(currentIndex, ListView.Beginning);
  }

  function pageBack() {
    decrementCurrentIndex();
    positionViewAtIndex(currentIndex, ListView.Beginning);
  }

  function pageTo(pageIndex) {
    currentIndex = pageIndex;
    positionViewAtIndex(currentIndex, ListView.Beginning);
  }

  anchors.fill: parent

  orientation: ListView.Horizontal
  snapMode: ListView.SnapOneItem
  highlightRangeMode: ListView.StrictlyEnforceRange
  cacheBuffer: width * pageCacheSize
  flickDeceleration: 50
}

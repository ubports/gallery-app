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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1

Item {
  id: albumPageLayoutALeft

  property variant mediaSourceList: (parent ? parent.mediaSourceList : null)

  property real topMargin: (parent ? parent.topMargin : 0)
  property real bottomMargin: (parent ? parent.topMargin : 0)
  property real gutterMargin: (parent ? parent.topMargin : 0)
  property real outerMargin: (parent ? parent.topMargin : 0)
  property real insideMargin: (parent ? parent.topMargin : 0)

  property bool isPreview: (parent ? parent.isPreview : false)

  width: (parent ? parent.width : 0)
  height: (parent ? parent.height : 0)

  FramePortrait {
    id: left

    anchors.fill: parent

    topMargin: albumPageLayoutALeft.topMargin
    bottomMargin: albumPageLayoutALeft.bottomMargin
    leftMargin: albumPageLayoutALeft.outerMargin
    rightMargin: albumPageLayoutALeft.gutterMargin

    mediaSource: (mediaSourceList ? mediaSourceList[0] : null)
    isPreview: albumPageLayoutALeft.isPreview
  }
}

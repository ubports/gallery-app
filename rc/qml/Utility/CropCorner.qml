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

import QtQuick 1.1

// A corner of a CropFrame.
Item {
  id: cropCorner

  signal dragged(real dx, real dy)
  signal dragStarted()
  signal dragCompleted()

  property bool isLeft: true
  property bool isTop: true
  property color color: "black"

  x: (isLeft ? -gu(3) : parent.width - gu(1))
  y: (isTop ? -gu(3) : parent.height - gu(1))
  width: gu(4)
  height: gu(4)

  Rectangle {
    x: 0
    y: (isTop ? 0 : gu(3))
    width: gu(4)
    height: gu(1)
    color: cropCorner.color
  }

  Rectangle {
    x: (isLeft ? 0 : gu(3))
    y: (isTop ? gu(1) : 0)
    width: gu(1)
    height: gu(3)
    color: cropCorner.color
  }

  CropDragArea {
    anchors.fill: parent

    onDragged: cropCorner.dragged(dx, dy)

    onDragStarted: cropCorner.dragStarted()

    onDragCompleted: cropCorner.dragCompleted()
  }
}

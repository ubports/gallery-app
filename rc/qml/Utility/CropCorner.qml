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

// A corner of a CropFrame.
Item {
  id: cropCorner

  signal dragged(real dx, real dy)
  signal dragStarted()
  signal dragCompleted()

  property bool isLeft: true
  property bool isTop: true
  property color color: "black"

  x: (isLeft ? -units.gu(3) : parent.width - units.gu(1))
  y: (isTop ? -units.gu(3) : parent.height - units.gu(1))
  width: units.gu(4)
  height: units.gu(4)

  Rectangle {
    x: 0
    y: (isTop ? 0 : units.gu(3))
    width: units.gu(4)
    height: units.gu(1)
    color: cropCorner.color
  }

  Rectangle {
    x: (isLeft ? 0 : units.gu(3))
    y: (isTop ? units.gu(1) : 0)
    width: units.gu(1)
    height: units.gu(3)
    color: cropCorner.color
  }

  CropDragArea {
    anchors.fill: parent

    onDragged: cropCorner.dragged(dx, dy)

    onDragStarted: cropCorner.dragStarted()

    onDragCompleted: cropCorner.dragCompleted()
  }
}

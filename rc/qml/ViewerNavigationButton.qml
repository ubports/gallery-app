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
 */

import QtQuick 1.1

Image {
  id: viewer_nav_image
  objectName: "viewer_nav_image"
  
  width: gu(3.25)
  height: gu(6)
  opacity: 0.7
  
  signal pressed()
  
  property bool is_forward: true
  
  source: (is_forward) ? "../img/viewer-forward.png" : "../img/viewer-back.png"
  
  MouseArea {
    anchors.fill: parent
    
    onClicked: viewer_nav_image.pressed()
  }
}


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
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1

GridView {
  id: photosOverview
  
  anchors.topMargin: units.gu(1)
  anchors.leftMargin: units.gu(1)
  anchors.rightMargin: units.gu(1)
  
  cellWidth: units.gu(12) + units.gu(0.75)
  cellHeight: units.gu(12) + units.gu(0.75)
  
  flickDeceleration: 800
  
  model: MediaCollectionModel {
    monitored: true
  }
  
  delegate: Item {
    width: photosOverview.cellWidth
    height: photosOverview.cellHeight
    
    Image {
      width: units.gu(12)
      height: units.gu(12)
      
      anchors.centerIn: parent
      
      source: mediaSource.galleryPreviewPath
      
      sourceSize.width: photosOverview.delegateWidth
      sourceSize.height: photosOverview.delegateHeight
      
      fillMode: Image.PreserveAspectCrop
      asynchronous: true
      cache: true
      smooth: true
    }
  }
}

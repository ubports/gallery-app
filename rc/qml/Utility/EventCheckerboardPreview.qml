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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import "../Components"

Item {
  id: eventCheckerboardPreview
  
  property alias ownerName: photo.ownerName
  property alias event: eventCard.event
  property alias mediaSource: photo.mediaSource
  property alias isSelected: eventCard.isSelected
  
  width: getDeviceSpecific("photoThumbnailWidth")
  height: getDeviceSpecific("photoThumbnailHeight")

  MattedPhotoPreview {
    id: photo
    
    anchors.fill: parent
    
    visible: (mediaSource) ? true : false
    isSelected: eventCheckerboardPreview.isSelected
  }
  
  EventCard {
    id: eventCard
    
    anchors.fill: parent
    
    visible: (event) ? true : false
  }
}

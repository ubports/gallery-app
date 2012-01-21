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

import QtQuick 1.1
import Gallery 1.0

ListView {
  id: eventTimeline
  
  property int elementWidth: 100
  property int elementHeight: 100
  property int eventCardWidth: 200
  property int eventCardHeight: elementHeight
  
  orientation: ListView.Vertical
  spacing: 4
  
  model: EventCollectionModel {
  }
  
  delegate: Rectangle {
    id: eventTimelineElement
    
    width: eventTimeline.width
    height: elementHeight
    
    EventCard {
      id: eventCard
      
      anchors.centerIn: parent
      
      width: eventCardWidth
      height: eventCardHeight
        
      z: 10
      
      event: model.event
    }
    
    ListView {
      anchors.fill: parent
      
      orientation: ListView.Horizontal
      spacing: 4
      
      interactive: false
      
      preferredHighlightBegin: (eventTimelineElement.width / 2) - 1
      preferredHighlightEnd: (eventTimelineElement.width / 2)
      highlightRangeMode: ListView.StrictlyEnforceRange
      
      model: MediaCollectionModel {
        forCollection: event
      }
      
      delegate: PhotoComponent {
        width: elementWidth
        height: elementHeight
        
        mediaSource: model.mediaSource
        isCropped: true
        isPreview: true
      }
      
      Component.onCompleted: {
        // when ready, move the index to the center item
        positionViewAtIndex(count / 2, ListView.Center);
      }
    }
  }
}

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
  
  signal activated(variant event)
  
  property int elementWidth: gu(8)
  property int elementHeight: gu(6)
  property int elementSpacing: gu(1)
  
  property int eventCardWidth: gu(24.75)
  property int eventCardHeight: elementHeight
  
  property int headTailCount: 7
  
  orientation: ListView.Vertical
  spacing: gu(1)
  
  cacheBuffer: height * 2
  
  model: EventCollectionModel {
  }
  
  delegate: Rectangle {
    id: eventTimelineElement
    
    width: eventTimeline.width
    height: elementHeight
    
    Row {
      id: leftList
      
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.right: eventCard.left
      
      spacing: elementSpacing
      
      Repeater {
        model: MediaCollectionModel {
          forCollection: event
          limit: Math.min(Math.ceil(rawCount / 2), headTailCount)
        }
        
        PhotoComponent {
          width: elementWidth
          height: elementHeight
          
          mediaSource: model.mediaSource
          isCropped: true
          isPreview: true
          ownerName: "EventTimeline"
        }
      }
    }
    
    EventCard {
      id: eventCard
      
      anchors.centerIn: parent
      anchors.leftMargin: elementSpacing
      anchors.rightMargin: elementSpacing
      
      width: eventCardWidth
      height: eventCardHeight
      
      event: model.event
      hasBottomSeparator: index != (eventTimeline.model.count - 1)
      
      MouseArea {
        anchors.fill: parent
        
        onClicked: activated(event)
      }
    }
    
    Row {
      id: rightList
      
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.left: eventCard.right
      
      spacing: elementSpacing
      
      Repeater {
        model: MediaCollectionModel {
          forCollection: event
          limit: Math.min(Math.floor(rawCount / 2), headTailCount)
          head: 0 - limit
        }
        
        PhotoComponent {
          width: elementWidth
          height: elementHeight
          
          mediaSource: model.mediaSource
          isCropped: true
          isPreview: true
          ownerName: "EventTimeline"
        }
      }
    }
  }
}

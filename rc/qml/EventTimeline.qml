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
  
  property int elementWidth: 64
  property int elementHeight: 48
  property int elementSpacing: 8
  
  property int eventCardWidth: 198
  property int eventCardHeight: elementHeight
  
  property int headTailCount: 7
  
  orientation: ListView.Vertical
  spacing: 8
  
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
          limit: headTailCount
        }
        
        PhotoComponent {
          width: elementWidth
          height: elementHeight
          
          mediaSource: model.mediaSource
          isCropped: true
          isPreview: true
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
          limit: headTailCount
          head: 0 - headTailCount
        }
        
        PhotoComponent {
          width: elementWidth
          height: elementHeight
          
          mediaSource: model.mediaSource
          isCropped: true
          isPreview: true
        }
      }
    }
  }
}

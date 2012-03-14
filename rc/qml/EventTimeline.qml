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
import "GalleryUtility.js" as GalleryUtility

ListView {
  id: eventTimeline
  
  signal activated(variant event)
  signal timedOut()
  
  property int elementWidth: gu(24)
  property int elementHeight: gu(19)
  property int elementSpacing: gu(1)
  
  property int eventCardWidth: elementWidth
  property int eventCardHeight: elementHeight
  
  property int headTailCount: 3
  
  function ensureIndexVisible(index, centered) {
    positionViewAtIndex(index, centered ? ListView.Center : ListView.Visible);
  }
  
  function getVisibleMediaSources() {
    var vi = GalleryUtility.getVisibleItems(eventTimeline, eventTimeline, function(child) {
      return child.objectName != "eventCard" && child.mediaSource;
    });
    
    var vo = [];
    for (var ctr = 0; ctr < vi.length; ctr++)
      vo[vo.length] = vi[ctr].mediaSource;
    
    return vo;
  }
  
  function getVisibleEvents() {
    var vi = GalleryUtility.getVisibleItems(eventTimeline, eventTimeline, function(child) {
      return child.objectName == "eventCard";
    });
    
    var ve = [];
    for (var ctr = 0; ctr < vi.length; ctr++)
      ve[ve.length] = vi[ctr].event;
    
    return ve;
  }
  
  function getRectOfEvent(event) {
    for (var ctr = 0; ctr < contentItem.children.length; ctr++) {
      var item = contentItem.children[ctr];
      if (item.objectName != "eventTimelineElement")
        continue;
      
      var eventCard = GalleryUtility.findChild(item, function(child) {
        return child.objectName == "eventCard" && child.event == event;
      });
      
      if (eventCard)
        return GalleryUtility.getRectRelativeTo(eventCard, eventTimeline);
    }
    
    return undefined;
  }
  
  function getRectOfMediaSource(mediaSource) {
    for (var ctr = 0; ctr < contentItem.children.length; ctr++) {
      var item = contentItem.children[ctr];
      if (item.objectName != "eventTimelineElement")
        continue;
      
      var photoComponent = GalleryUtility.findChild(item, function(child) {
        return child.mediaSource == mediaSource;
      });
      
      if (photoComponent)
        return GalleryUtility.getRectRelativeTo(photoComponent, eventTimeline);
    }
    
    return undefined;
  }
  
  orientation: ListView.Vertical
  spacing: elementSpacing
  
  cacheBuffer: height * 2
  
  model: EventCollectionModel {
  }
  
  delegate: Item {
    id: eventTimelineElement
    objectName: "eventTimelineElement"
    
    property int index: model.index
    property Event event: model.event
    
    width: eventTimeline.width
    height: elementHeight
    
    Row {
      id: leftList
      
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.right: eventCard.horizontalCenter
      
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
    
    Row {
      id: rightList
      
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.left: eventCard.horizontalCenter
      
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
    
    EventCard {
      id: eventCard
      objectName: "eventCard"
      
      anchors.centerIn: parent
      
      width: eventCardWidth
      height: eventCardHeight
      
      event: model.event
      
      MouseArea {
        anchors.fill: parent
        
        onClicked: {
          timeoutTimer.stop();
          activated(event);
        }
      }
    }
  }

  onVisibleChanged: {
    if (visible)
      timeoutTimer.restart();
  }

  onContentYChanged: timeoutTimer.restart()

  Timer {
    id: timeoutTimer

    interval: 3000
    running: false

    onTriggered: eventTimeline.timedOut()
  }
}

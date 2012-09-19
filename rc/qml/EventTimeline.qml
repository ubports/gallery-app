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

import QtQuick 1.1
import Gallery 1.0
import "../js/GalleryUtility.js" as GalleryUtility
import "../js/Gallery.js" as Gallery
import "Components"

Item {
  id: eventTimeline
  
  signal activated(variant event)
  signal movementStarted()
  signal movementEnded()

  property alias model: list.model
  property alias contentY: list.contentY
  
  property int elementWidth: Gallery.getDeviceSpecific('photoThumbnailWidthTimeline')
  property int elementHeight: Gallery.getDeviceSpecific('photoThumbnailHeightTimeline')
  property int verticalGutter: gu(2)
  
  property real topExtraGutter: 0
  property real bottomExtraGutter: 0
  property real leftExtraGutter: 0
  property real rightExtraGutter: 0

  property int eventCardWidth: elementWidth
  property int eventCardHeight: elementHeight
  
  property int headTailCount: 3
  
  function ensureIndexVisible(index, centered) {
    list.positionViewAtIndex(index, centered ? ListView.Center : ListView.Visible);
  }
  
  function scrollToTop() {
    scroller.restart();
  }

  function getVisibleMediaSources() {
    var vi = GalleryUtility.getVisibleItems(list, eventTimeline, function(child) {
      return child.objectName != "eventCard" && child.mediaSource;
    });
    
    var vo = [];
    for (var ctr = 0; ctr < vi.length; ctr++)
      vo[vo.length] = vi[ctr].mediaSource;
    
    return vo;
  }
  
  function getVisibleEvents() {
    var vi = GalleryUtility.getVisibleItems(list, eventTimeline, function(child) {
      return child.objectName == "eventCard";
    });
    
    var ve = [];
    for (var ctr = 0; ctr < vi.length; ctr++)
      ve[ve.length] = vi[ctr].event;
    
    return ve;
  }
  
  // Returns a rect with additional z property.
  function getGeometryOfEvent(event) {
    for (var ctr = 0; ctr < list.contentItem.children.length; ctr++) {
      var item = list.contentItem.children[ctr];
      if (item.objectName != "eventTimelineElement")
        continue;
      
      var eventCard = GalleryUtility.findChild(item, function(child) {
        return child.objectName == "eventCard" && child.event == event;
      });
      
      if (eventCard) {
        var geom = GalleryUtility.getRectRelativeTo(eventCard, eventTimeline);
        geom.z = eventCard.z;
        return geom;
      }
    }
    
    return undefined;
  }
  
  // Returns a rect with additional z property.
  function getGeometryOfMediaSource(mediaSource) {
    for (var ctr = 0; ctr < list.contentItem.children.length; ctr++) {
      var item = list.contentItem.children[ctr];
      if (item.objectName != "eventTimelineElement")
        continue;
      
      var photoComponent = GalleryUtility.findChild(item, function(child) {
        return child.mediaSource == mediaSource;
      });
      
      if (photoComponent) {
        var geom = GalleryUtility.getRectRelativeTo(photoComponent, eventTimeline);
        geom.z = photoComponent.z;
        return geom;
      }
    }
    
    return undefined;
  }

  clip: true
  
  ListView {
    id: list

    anchors.fill: parent
    anchors.topMargin: topExtraGutter + verticalGutter
    anchors.bottomMargin: bottomExtraGutter + verticalGutter
    anchors.leftMargin: leftExtraGutter
    anchors.rightMargin: rightExtraGutter

    orientation: ListView.Vertical
    spacing: verticalGutter

    cacheBuffer: height * 2

    model: EventCollectionModel {
    }

    delegate: Item {
      id: eventTimelineElement
      objectName: "eventTimelineElement"

      property int index: model.index
      property Event event: model.event

      width: list.width
      height: elementHeight
      
      Repeater {
        id: leftRepeater

        model: MediaCollectionModel {
          id: leftRepeaterModel

          forCollection: event
          limit: Math.min(Math.ceil(rawCount / 2), headTailCount)
        }

        MattedPhotoPreview {
          id: leftThumbnail

          property int position: headTailCount - leftRepeaterModel.count + index

          x: {
            var x = eventCard.x - elementWidth / 2;
            if (position < 2)
              x -= Gallery.getDeviceSpecific('timelineFirstPhotoDistance');
            if (position < 1)
              x -= Gallery.getDeviceSpecific('timelineSecondPhotoDistance');
            return x;
          }
          y: eventCard.y
          z: position + 1
          width: elementWidth
          height: elementHeight

          mediaSource: model.mediaSource
          ownerName: "EventTimeline"

          opacity: Gallery.EVENT_TIMELINE_MEDIA_SOURCE_OPACITY
        }
      }

      Repeater {
        id: rightRepeater

        model: MediaCollectionModel {
          forCollection: event
          limit: Math.min(Math.floor(rawCount / 2), headTailCount)
          head: 0 - limit
        }

        MattedPhotoPreview {
          id: rightThumbnail

          x: {
            var x = eventCard.x + elementWidth / 2;
            if (index > 0)
              x += Gallery.getDeviceSpecific('timelineFirstPhotoDistance');
            if (index > 1)
              x += Gallery.getDeviceSpecific('timelineSecondPhotoDistance');
            return x;
          }
          y: eventCard.y
          z: headTailCount - index
          width: elementWidth
          height: elementHeight

          mediaSource: model.mediaSource
          ownerName: "EventTimeline"

          opacity: Gallery.EVENT_TIMELINE_MEDIA_SOURCE_OPACITY
        }
      }

      EventCard {
        id: eventCard
        objectName: "eventCard"

        anchors.centerIn: parent

        z: headTailCount + 1

        width: eventCardWidth
        height: eventCardHeight

        event: model.event
        
        opacity: Gallery.EVENT_TIMELINE_EVENT_CARD_OPACITY
      }
      
      MouseArea {
        anchors.fill: parent
        
        onClicked: activated(event)
      }
    }

    onMovementStarted: eventTimeline.movementStarted();

    onMovementEnded: eventTimeline.movementEnded();
  }

  NumberAnimation {
    id: scroller

    target: list
    property: "contentY"
    to: 0

    easing.type: Easing.OutQuad
    duration: 200
  }
}

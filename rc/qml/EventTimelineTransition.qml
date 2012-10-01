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
import "../js/Gallery.js" as Gallery
import "../js/GalleryUtility.js" as GalleryUtility
import "Utility"
import "Widgets"

Item {
  id: eventTimelineTransition
  
  property EventCheckerboard checkerboard
  property EventTimeline timeline
  property GalleryOverviewNavigationBar navigationBar
  
  property int duration: Gallery.SLOW_DURATION
  
  // readonly
  property real itemWidth: checkerboard.itemWidth
  property real itemHeight: checkerboard.itemHeight
  property real gutterWidth: checkerboard.gutterWidth
  property real gutterHeight: checkerboard.gutterHeight
  property real delegateWidth: checkerboard.delegateWidth
  property real delegateHeight: checkerboard.delegateHeight
  property real topExtraGutter: checkerboard.topExtraGutter
  property real bottomExtraGutter: checkerboard.bottomExtraGutter
  property real leftExtraGutter: checkerboard.leftExtraGutter
  property real rightExtraGutter: checkerboard.rightExtraGutter
  
  // internal
  property EventOverviewModel model: EventOverviewModel {}
  property int xMax: width / delegateWidth
  property int gridWidth: Gallery.getDeviceSpecific('photoThumbnailWidth')
  property int gridHeight: Gallery.getDeviceSpecific('photoThumbnailHeight')
  property int timelineWidth: Gallery.getDeviceSpecific('photoThumbnailWidthTimeline')
  property int timelineHeight: Gallery.getDeviceSpecific('photoThumbnailHeightTimeline')

  
  states: [
    State {
      name: "grid"
    },
    
    State {
      name: "timeline"
    }
  ]
  
  state: "grid"
  
  // public
  function toTimeline(event) {
    start(event, "grid", "timeline");
  }
  
  // public
  function toOverview(event) {
    start(event, "timeline", "grid");
  }

  // public
  function backToOverview() {
    // Uses the previously-saved event.
    start(event, "timeline", "grid");
  }
  
  // internal
  property Event event
  property string starting
  property string ending
  
  // internal
  function start(event, starting, ending) {
    // Because it's desireable for the selected event to be visible in the timeline
    // view when the transition occurs, need to ensure it's ready.  However,
    // can't do it and start the transition because the event loop has
    // not had a chance to run.  By using a Timer, we give the loop a chance to
    // process the request before starting the transition
    if (starting == "grid")
      timeline.ensureIndexVisible(timeline.model.indexOf(event), true);
    else
      checkerboard.ensureIndexVisible(checkerboard.model.indexOf(event), true);
    
    eventTimelineTransition.event = event;
    eventTimelineTransition.starting = starting;
    eventTimelineTransition.ending = ending;
    
    delayedTimer.start();
  }
  
  // internal
  function delayedStart() {
    if (!checkerboard || !timeline)
      return;
    
    state = starting;
    
    // Only need to fetch MediaSources; the EventOverviewModel will automatically
    // create Event Cards
    var vc = checkerboard.getVisibleMediaSources();
    var vt = timeline.getVisibleMediaSources();
    
    checkerboard.visible = false;
    timeline.visible = false;
    
    // clear model before changing state
    model.clear();
    
    // populate with items visible in the checkerboard; their locations will be
    // determined as the delegates are created
    for (var ctr = 0; ctr < vc.length; ctr++)
      model.add(vc[ctr]);
    
    // add items visible in the timline but not in the checkerboard; they
    // will emerge from their appropiate event card and dissolve in/out as they
    // fly in to/out of position
    for (var ctr = 0; ctr < vt.length; ctr++) {
      if (vc.indexOf(vt[ctr]) == -1)
        model.add(vt[ctr]);
    }
    
    // start the transition
    visible = true;
    state = ending;
  }
  
  Timer {
    id: delayedTimer
    
    interval: 20
    repeat: false
    
    onTriggered: eventTimelineTransition.delayedStart()
  }
  
  Repeater {
    id: repeater
    
    anchors.fill: parent

    anchors.topMargin: topExtraGutter
    anchors.bottomMargin: bottomExtraGutter
    anchors.leftMargin: leftExtraGutter
    anchors.rightMargin: rightExtraGutter
    
    model: eventTimelineTransition.model
    
    delegate: Item {
      id: cell
      
      // This is necessary to expose the index properties externally
      property int index: model.index
      property int gridX
      property int gridY
      property int timelineX
      property int timelineY
      property bool isEvent: model.typeName == "Event"
      property bool hidesUnderEvent: false
      property bool disappears: false
      
      width: itemWidth
      height: itemHeight
      
      z: isEvent ? 1 : 0
      
      state: eventTimelineTransition.state
      
      states: [
        State {
          name: "grid"
          
          PropertyChanges {
            target: cell
            x: gridX
            y: gridY
            width: gridWidth
            height: gridHeight
            opacity: 1.0
          }
        },
        
        State {
          name: "timeline"
          
          PropertyChanges {
            target: cell
            x: timelineX
            y: timelineY
            width: timelineWidth
            height: timelineHeight
            opacity: {
              if (disappears || hidesUnderEvent)
                return 0.0;
              else if (isEvent)
                return Gallery.EVENT_TIMELINE_EVENT_CARD_OPACITY;
              else
                return Gallery.EVENT_TIMELINE_MEDIA_SOURCE_OPACITY;
            }
          }
        }
      ]
      
      transitions: [
        Transition {
          from: "grid"
          to: "timeline"
          
          SequentialAnimation {
            PropertyAnimation {
              properties: "x,y,width,height,opacity"
              easing.type: Easing.InQuint
              duration: eventTimelineTransition.duration
            }
            
            ScriptAction {
              script: {
                eventTimelineTransition.visible = false;
                timeline.visible = true;
              }
            }
          }
        },
        
        Transition {
          from: "timeline"
          to: "grid"
          
          SequentialAnimation {
            PropertyAnimation {
              properties: "x,y,width,height,opacity"
              easing.type: Easing.InQuint
              duration: eventTimelineTransition.duration
            }
            
            ScriptAction {
              script: {
                eventTimelineTransition.visible = false;
                checkerboard.visible = true;
              }
            }
          }
        }
      ]
      
      Component.onCompleted: {
        if (!checkerboard || !timeline)
          return;
        
        // although could use checkerboard.getRectOfObject(), that method
        // uses a potentially expensive searching algorithm; plus, if the
        // object is not loaded into the GridView, still need to know where
        // it would be located *if* it was.  Its index in the model and the
        // dimensions of the delegates and viewing area means that is
        // deterministic for all objects

        // Note that we have to compensate for the photo being slightly smaller
        // than the GridView delegate (the gutter), and the
        // top/leftExtraGutters.
        var widthOffset = gutterWidth / 2 + leftExtraGutter;
        var heightOffset = gutterHeight / 2 + topExtraGutter;

        var cindex = checkerboard.model.indexOf(model.object);
        if (cindex >= 0) {
          // As view scrolls vertically, x coordinate needs no translation
          gridX = (cindex % xMax) * delegateWidth + widthOffset;
          
          // translate y coordinate according to checkerboard viewport
          gridY = (Math.floor(cindex / xMax) * delegateHeight) - checkerboard.contentY + heightOffset;
        } else {
          console.log("Unable to find index for", model.object);
          disappers = true;
        }
        
        // Formula for deriving timeline location:
        // 1. If Event, fly to EventCard position
        // 2. If Photo and visible in Event Timeline, fly to PhotoComponent position
        // 3. If Photo and not visible in Event Timeline, fly to assoc. EventCard
        
        var geom = !isEvent ? timeline.getGeometryOfMediaSource(model.mediaSource) : undefined;
        if (!geom && !isEvent) {
          geom = timeline.getGeometryOfEvent(model.mediaSource.event);
          hidesUnderEvent = true;
        } else if (isEvent) {
          geom = timeline.getGeometryOfEvent(model.object);
        }
        
        if (geom) {
          if (!hidesUnderEvent)
            z = geom.z;
          geom = GalleryUtility.translateRect(geom, timeline, eventTimelineTransition);
          timelineX = geom.x;
          timelineY = geom.y;
        } else {
          // this can happen if the event line hasn't been instantiated yet
          // in the timeline (due to ListView only instantiating delegates
          // on-demand) ... this is okay, not vital for this transition to
          // look good
          disappears = true;
          timelineX = gridX;
          timelineY = gridY;
        }
      }
      
      EventCheckerboardPreview {
        anchors.fill: parent

        ownerName: "EventTimelineTransition"

        mediaSource: (model.typeName == "MediaSource") ? model.mediaSource : undefined
        event: (model.typeName == "Event") ? model.object : undefined
      }
    }
  }
}

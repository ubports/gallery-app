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

Rectangle {
  id: eventTimelineTransition
  
  property EventCheckerboard checkerboard
  property EventTimeline timeline
  
  property Component delegate
  
  property int duration: 750
  
  // readonly
  // TODO: Make constants used from Checkerboard or EventCheckerboard
  property int widthSansStroke: 206
  property int heightSansStroke: 156
  property int widthWithStroke: 198
  property int heightWithStroke: 148
  
  // internal
  property EventOverviewModel model: EventOverviewModel {}
  property int xMax: width / widthSansStroke
  
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
    
    model: eventTimelineTransition.model
    
    delegate: Rectangle {
      id: cell
      
      // This is necessary to expose the index properties externally
      property int index: model.index
      property int gridX
      property int gridY
      property int timelineX
      property int timelineY
      property bool isEvent: model.typeName == "Event"
      property bool hidesUnderEvent: false
      
      width: widthSansStroke
      height: heightSansStroke
      
      z: isEvent ? 1 : 0
      
      state: eventTimelineTransition.state
      
      states: [
        State {
          name: "grid"
          
          PropertyChanges {
            target: cell
            x: gridX
            y: gridY
            opacity: 1.0
          }
        },
        
        State {
          name: "timeline"
          
          PropertyChanges {
            target: cell
            x: timelineX
            y: timelineY
            opacity: (!isEvent && hidesUnderEvent) ? 0.0 : 1.0
          }
        }
      ]
      
      transitions: [
        Transition {
          from: "grid"
          to: "timeline"
          
          SequentialAnimation {
            PropertyAnimation {
              properties: "x,y,opacity"
              easing.type: Easing.InOutQuad
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
              properties: "x,y,opacity"
              easing.type: Easing.InOutQuad
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
        var cindex = checkerboard.model.indexOf(model.object);
        if (cindex >= 0) {
          // As view scrolls vertically, x coordinate needs no translation
          gridX = (cindex % xMax) * widthSansStroke;
          
          // translate y coordinate according to checkerboard viewport
          gridY = (Math.floor(cindex / xMax) * heightSansStroke) - checkerboard.contentY;
        } else {
          console.log("Unable to find index for", model.object);
          visible = false;
        }
        
        // Formula for deriving timeline location:
        // 1. If Event, fly to EventCard position
        // 2. If Photo and visible in Event Timeline, fly to PhotoComponent position
        // 3. If Photo and not visible in Event Timeline, fly to assoc. EventCard
        
        var rect = !isEvent ? timeline.getRectOfMediaSource(model.mediaSource) : undefined;
        if (!rect && !isEvent) {
          rect = timeline.getRectOfEvent(model.mediaSource.event);
          hidesUnderEvent = true;
        } else if (isEvent) {
          rect = timeline.getRectOfEvent(model.object);
        }
        
        if (rect) {
          timelineX = rect.x;
          timelineY = rect.y;
        } else {
          console.log("Unable to find timeline location of", model.object);
          visible = false;
        }
      }
      
      Loader {
        id: loader
        
        property variant modelData: model
        
        anchors.centerIn: parent
        
        width: widthWithStroke
        height: heightWithStroke
        
        sourceComponent: eventTimelineTransition.delegate
        
        onLoaded: {
          cell.isEvent = (item.event) ? true : false;
        }
      }
    }
  }
}

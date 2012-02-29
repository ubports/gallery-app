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
  
  property int duration: 500
  
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
  function toTimeline() {
    start("grid", "timeline");
  }
  
  // public
  function toOverview() {
    start("timeline", "grid");
  }
  
  // internal
  function start(starting, ending) {
    if (!checkerboard || !timeline)
      return;
    
    checkerboard.visible = false;
    timeline.visible = false;
    
    // clear model before changing state
    model.clear();
    
    state = starting;
    
    var vt = timeline.getVisibleMediaSources();
    var vc = checkerboard.getVisibleMediaSources();
    
    // populate with items visible in the timeline; their locations will be
    // determined as the delegates are created
    for (var ctr = 0; ctr < vt.length; ctr++)
      model.add(vt[ctr]);
    
    // add items visible in the checkerboard but not in the timeline; they
    // will emerge from their appropiate event card and dissolve in as they
    // fly into position
    for (var ctr = 0; ctr < vc.length; ctr++) {
      if (vt.indexOf(vc[ctr]) == -1)
        model.add(vc[ctr]);
    }
    
    // start the transition
    visible = true;
    state = ending;
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
        
        var rect = checkerboard.getRectOfObject(model.object);
        if (rect) {
          gridX = rect.x;
          gridY = rect.y;
        } else {
          var index = checkerboard.model.indexOf(model.object);
          if (index >= 0) {
            // place photos that are not visible in the checkerboard
            // (but visible in the timeline) off-screen so they fly into place
            gridX = (index % xMax) * widthSansStroke;
            gridY = eventTimeline.y + (Math.floor(index / xMax) * heightSansStroke)
          } else {
            console.log("Unable to find index for", model.object);
            visible = false;
          }
        }
        
        // Formula for deriving timeline location:
        // 1. If Event, fly to EventCard
        // 2. If Photo and visible in Event Timeline, fly to PhotoComponent
        // 3. If not visible in Event Timeline, fly to assoc. EventCard
        
        rect = !isEvent ? timeline.getRectOfMediaSource(model.mediaSource) : undefined;
        if (!rect) {
          if (!isEvent) {
            rect = timeline.getRectOfEvent(model.mediaSource.event);
            hidesUnderEvent = true;
          } else {
            rect = timeline.getRectOfEvent(model.object);
          }
        }
        
        if (rect) {
          timelineX = rect.x;
          timelineY = rect.y;
        } else {
          console.log("Unable to find timeline location of", model.object);
          visible = false;
        }
        
        // place in starting location according to state
        if (state == "grid") {
          x = gridX;
          y = gridY;
        } else {
          x = timelineX;
          y = timelineY;
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

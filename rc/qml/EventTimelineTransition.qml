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

Rectangle {
  id: eventTimelineTransition
  
  property alias model: repeater.model
  property Component delegate
  
  property int widthSansStroke: 206
  property int heightSansStroke: 156
  property int widthWithStroke: 198
  property int heightWithStroke: 148
  
  property int xMax: width / widthSansStroke
  
  property EventCheckerboard checkerboard
  property EventTimeline timeline
  
  states: [
    State {
      name: "grid"
    },
    
    State {
      name: "timeline"
    }
  ]
  
  state: "grid"
  
  function toTimeline() {
    if (!checkerboard) {
      console.log("checkerboard is undefined");
      
      return;
    }
    
    checkerboard.visible = false;
    visible = true;
    
    // clear model before changing state
    model.clear();
    
    state = "grid";
    
    var vc = checkerboard.getVisibleObjects();
    var vt = timeline.getVisibleObjects();
    
    // populate with items visible in the checkerboard; they're locations will
    // be determined as the delegates are created
    for (var ctr = 0; ctr < vc.length; ctr++)
      model.add(vc[ctr]);
    
    // add items visible in the timeline but not visible in the checkerboard;
    // these will fly in from off-screen from the appropriate direction
    for (var ctr = 0; ctr < vt.length; ctr++) {
      if (vc.indexOf(vt[ctr]) == -1)
        model.add(vt[ctr]);
    }
    
    // start the transition
    state = "timeline";
  }
  
  Repeater {
    id: repeater
    
    anchors.fill: parent
    
    delegate: Rectangle {
      id: cell
      
      // This is necessary to expose the index properties externally
      property int index: model.index
      property int gridX
      property int gridY
      property int timelineX
      property int timelineY
      property bool isEvent: false
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
              duration: 500
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
              duration: 500
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
            gridY = eventTimeline.y + eventTimeline.height;
          } else {
            console.log("Unable to find index for", model.object);
          }
        }
        
        // Formula for deriving final location:
        // 1. If Event, fly to EventCard
        // 2. If Photo and visible in Event Timeline, fly to PhotoComponent
        // 3. If not visible in Event Timeline, fly to assoc. EventCard
        
        rect = (model.mediaSource) ? timeline.getRectOfMediaSource(model.mediaSource) : undefined;
        if (!rect) {
          if (model.mediaSource) {
            rect = timeline.getRectOfEvent(model.mediaSource.event);
            hidesUnderEvent = true;
          } else {
            rect = timeline.getRectOfEvent(model.object.event);
          }
        }
        
        if (rect) {
          timelineX = rect.x;
          timelineY = rect.y;
        } else {
          //console.log("using fake timeline coords for", model.object);
          timelineX = 1600;
          timelineY = 1600;
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

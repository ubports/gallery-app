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
 *
 * Inspired by code written by Tommi Laukkanen, available at
 * http://www.substanceofcode.com/downloads/flipboard_page_turn_with_qml.zip
 */

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  id: pageFlipAnimation
  
  property AlbumPage leftPage
  property AlbumPage rightPage
  
  property bool pageFlipped: false
  
  property bool leftToRight: true
  property int durationMsec: 1000
  
  function start() {
    if (state != "" || clipper.trigger) {
      console.log("Blocking start of page flip animation");
      
      return;
    }
    
    // reset
    pageFlipped = false;
    clipper.trigger = false;
    clipper.x = leftToRight ? pageFlipAnimation.width / 2 : 0
    flippable.x = leftToRight ? -(pageFlipAnimation.width / 2) : 0
    flippable.rotation.angle = leftToRight ? 0.0 : -180.0;
    
    // start animation
    clipper.trigger = true;
  }
  
  Item {
    width: parent.width / 2
    height: parent.height
    
    clip: true
    
    Rectangle {
      x: 0
      y: 0
      width: parent.width
      height: parent.height
      
      AlbumPageComponent {
        id: leftPageBackground
        
        albumPage: leftPage
        
        width: pageFlipAnimation.width
        height: pageFlipAnimation.height
      }
    }
  }
  
  Item {
    x: parent.width / 2
    width: parent.width / 2
    height: parent.height
    
    clip: true
    
    Rectangle {
      x: -(parent.width / 2)
      y: 0
      width: parent.width
      height: parent.height
      
      AlbumPageComponent {
        id: rightPageBackground
        
        albumPage: rightPage
        
        x: parent.x
        y: parent.y
        width: pageFlipAnimation.width
        height: pageFlipAnimation.height
      }
    }
  }
  
  Item {
    id: clipper
    
    property bool trigger: false
    
    x: leftToRight ? parent.width / 2 : 0
    width: parent.width / 2
    height: parent.height
    
    clip: true
    
    Flipable {
      id: flippable
      
      x: leftToRight ? -(pageFlipAnimation.width / 2) : 0
      width: pageFlipAnimation.width
      height: pageFlipAnimation.height
      
      front: Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        
        AlbumPageComponent {
          id: leftPageAnimated
          
          albumPage: leftPage
          
          width: pageFlipAnimation.width
          height: pageFlipAnimation.height
        }
      }
      
      back: Rectangle {
        x: 0
        y: 0
        width: parent.width
        height: parent.height
        
        AlbumPageComponent {
          id: rightPageAnimated
          
          albumPage: rightPage
          
          width: pageFlipAnimation.width
          height: pageFlipAnimation.height
        }
      }
        
      transform: [
        Rotation {
          id: rotation
          
          origin.x: pageFlipAnimation.width / 2
          origin.y: pageFlipAnimation.height / 2
          
          axis.x: 0
          axis.y: 1
          axis.z: 0
          
          angle: leftToRight ? 0.0 : -180.0
        }
      ]
      
      states: State {
        name: "running"
        PropertyChanges { target: rotation; angle: leftToRight ? -180.0 : 0.0; }
        when: clipper.trigger
      }
      
      transitions: Transition {
        to: "running"
        
        SequentialAnimation {
          ParallelAnimation {
            NumberAnimation {
              target: rotation
              property: "angle"
              duration: durationMsec
              easing.type: Easing.InOutCubic
            }
            
            // halfway through the animation, move the clipping region to the
            // other half of the region
            SequentialAnimation {
              PauseAnimation {
                duration: durationMsec / 2
              }
              ScriptAction {
                script: {
                  if(clipper.x != 0) {
                    clipper.x = 0;
                    flippable.x = 0;
                  } else {
                    clipper.x = pageFlipAnimation.width / 2;
                    flippable.x = -pageFlipAnimation.width / 2;
                  }
                }
              }
            }
          }
          
          // Signal that the page flip is complete
          PropertyAction {
            target: pageFlipAnimation
            property: "pageFlipped"
            value: true
          }
          
          // Reset state for next page flip
          PropertyAction {
            target: clipper
            property: "trigger"
            value: false
          }
        }
      }
    }
  }
}

/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1
import org.yorba.qt.qmlmediamodel 1.0

Rectangle {
  id: checkerboard_surface
  objectName: "checkerboard_surface"
  
  signal switch_to_album_view()
  signal switch_to_photo_view()
  
  anchors.fill: parent
  
  TopBar {
    id: topbar
    objectName: "topbar"
  }
  
  NavToolbar {
    id: navbar
    objectName: "navbar"
    
    anchors.top: topbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    
    BinaryTabGroup {
      id: album_view_switcher
      objectName: "album_view_switcher"
      
      x: parent.width / 2 - width / 2
      y: 4
      
      tab0_title: "Photos"
      tab1_title: "Albums"
      
      onTab0_activated: switch_to_album_view()
      onTab1_activated: switch_to_photo_view()
    }
    
    NavButton {
      id: deselect
      objectName: "deselect"
      
      anchors.right:  cancel_selecting.left
      
      title: "deselect"
      visible: false
      
      onPressed: {
        checkerboard.unselect_all();
      }
    }
    
    NavButton {
      id: cancel_selecting
      objectName: "cancel_selecting"
      
      anchors.right:  parent.right
      
      title: "done"
      visible: false
      
      onPressed: {
        checkerboard.state = "normal";
        checkerboard.unselect_all();
      }
    }
  }
  
  Rectangle {
    id: checkerboard
    objectName: "checkerboard"
    
    signal activated(int number)
    signal selection_toggled(int number)
    signal unselect_all()
    signal create_album_from_selected()
    
    color: "white"
    clip: true
    anchors.top: navbar.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins: 24
    
    state: "normal"
    states: [
      State { 
        name: "normal"
        PropertyChanges { target: cancel_selecting; visible: false }
        PropertyChanges { target: deselect; visible: false }
        PropertyChanges { target: toolbar; opacity: 0 }
      },
      State { 
        name: "to-selecting" 
        PropertyChanges { target: cancel_selecting; visible: true }
        PropertyChanges { target: deselect; visible: true }
        PropertyChanges { target: toolbar; opacity: 1 }
      },
      State { 
        name: "selecting"
        PropertyChanges { target: cancel_selecting; visible: true }
        PropertyChanges { target: deselect; visible: true }
        PropertyChanges { target: toolbar; opacity: 1 }
      }
    ]
    
    GridView {
      id: grid
      objectName: "grid"
      
      width: parent.width
      height: parent.height
      
      // spec calls for 206x156, 198x148 without stroke -- split the difference
      // here to not double up space in the middle gutters
      //
      // TODO: Verify that the edge gutters are properly spaced
      cellWidth: 202
      cellHeight: 152
      
      model: gridModel
      
      delegate: Row {
        Rectangle {
          width: 202
          height: 152
          
          Rectangle {
            width: 198
            height: 148
            
            anchors.centerIn: parent
            
            Image {
              source: preview_path
              
              anchors.centerIn: parent
              
              asynchronous: true
              cache: true
              smooth: true
              
              MouseArea {
                anchors.fill: parent
                
                onPressAndHold: {
                  // Press-and-hold activates selection mode,
                  // but need to differentiate in onReleased whether it's a mode
                  // change or a selection/activation
                  if (checkerboard.state == "normal")
                    checkerboard.state = "to-selecting";
                }
                
                onReleased: {
                  // See onPressAndHold for note on logic behind state changes
                  if (checkerboard.state == "normal")
                    checkerboard.activated(media_number)
                  else if (checkerboard.state == "to-selecting")
                    checkerboard.state = "selecting";
                  else if (checkerboard.state == "selecting")
                    checkerboard.selection_toggled(media_number);
                }
              }
              
              Image {
                source: "selected-media.png"
                
                anchors.top: parent.top
                anchors.right: parent.right
                
                visible: is_selected
                
                asynchronous: true
                cache: true
                smooth: true
              }
            }
          }
        }
      }
    }
  }
  
  NavToolbar {
    id: toolbar
    objectName: "toolbar"
    
    z: 10
    anchors.bottom: parent.bottom
    
    NavButton {
      id: create_album
      objectName: "create_album"
      
      anchors.right: parent.right
      
      title: "album"
    }
  }
}

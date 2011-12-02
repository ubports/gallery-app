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

Rectangle {
  id: tab_wrapper
  objectName: "tab_wrapper"
  
  property string title: ""
  
  height: 44
  
  signal activated()
  
  states: [
    State { name: "selected";
      PropertyChanges { target: selected_tab_image; visible: true; }
      PropertyChanges { target: deselected_tab_image; visible: false; }
      PropertyChanges { target: tab_wrapper; z: 10; }
    },

    State { name: "deselected";
      PropertyChanges { target: selected_tab_image; visible: false; }
      PropertyChanges { target: deselected_tab_image; visible: true; }
      PropertyChanges { target: tab_wrapper; z: 0; }
    }
  ]
  
  state: "deselected"
  
  Image {
    id: selected_tab_image
    objectName: "selected_tab_image"
    
    source: "selected-tab.png"
    
    Text {
      text: tab_wrapper.title
      anchors.centerIn: parent
      color: "#657CA9"
    }
    
    MouseArea {
      anchors.fill: parent
      
      onClicked: activated()
    }
  }
  
  Image {
    id: deselected_tab_image
    objectName: "deselected_tab_image"
    
    source: "deselected-tab.png"

    Text {
      text: tab_wrapper.title
      anchors.centerIn: parent
      color: "#657CA9"
    }

    MouseArea {
      anchors.fill: parent
      
      onClicked: activated()
    }
  }
}


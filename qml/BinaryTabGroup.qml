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

import QtQuick 1.1;

Rectangle {
  id: binary_tab_group
  objectName: "binary_tab_group"
  
  property string tab0_title: "Tab 0"
  property string tab1_title: "Tab 1"
  
  signal tab0_activated()
  signal tab1_activated()
  
  width: 296
  
  states: [
    State { name: "tab0_active";
      PropertyChanges { target: tab0; state: "selected"; }
      PropertyChanges { target: tab1; state: "deselected"; }
    },
    
    State { name: "tab1_active";
      PropertyChanges { target: tab0; state: "deselected" }
      PropertyChanges { target: tab1; state: "selected"; }
    }
  ]
  
  state: "tab0_active"
  
  Tab {
    id: tab0;
    objectName: "tab0";
    
    title: binary_tab_group.tab0_title
    
    x: 0
    
    onActivated: {
      if (binary_tab_group.state == "tab0_active")
        binary_tab_group.state = "tab1_active"
      else
        binary_tab_group.state = "tab0_active"
                   
      tab0_activated()
    }
  }
  
  Tab {
    id: tab1;
    objectName: "tab1";
    
    title: binary_tab_group.tab1_title
    
    x: 140
    
    onActivated: {
      if (binary_tab_group.state == "tab0_active")
        binary_tab_group.state = "tab1_active"
      else
        binary_tab_group.state = "tab0_active"
                     
      tab1_activated()
    }
  }
}


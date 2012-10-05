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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0;

Item {
  id: binary_tab_group
  objectName: "binary_tab_group"
  
  signal tab0Activated()
  signal tab1Activated()

  property alias tab0Title: tab0.title
  property alias tab1Title: tab1.title

  // readonly
  property bool isTab0Active: state == "tab0_active"
  
  width: childrenRect.width
  height: childrenRect.height

  states: [
    State { name: "tab0_active";
      PropertyChanges { target: tab0; state: "selected"; }
      PropertyChanges { target: tab1; state: "deselected"; }
    },
    
    State { name: "tab1_active";
      PropertyChanges { target: tab0; state: "deselected"; }
      PropertyChanges { target: tab1; state: "selected"; }
    }
  ]
  
  state: "tab0_active"
  
  Row {
    Tab {
      id: tab0;

      selectedBackgroundSource: "img/tab-left-active.png"
      deselectedBackgroundSource: "img/tab-left-inactive.png"

      onActivated: {
        binary_tab_group.state = "tab0_active";
        tab0Activated();
      }
    }

    Tab {
      id: tab1;

      selectedBackgroundSource: "img/tab-right-active.png"
      deselectedBackgroundSource: "img/tab-right-inactive.png"

      onActivated: {
        binary_tab_group.state = "tab1_active";
        tab1Activated();
      }
    }
  }
}

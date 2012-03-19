/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1
import "GalleryUtility.js" as GalleryUtility
import "../Capetown"

PopupBox {
  id: popupMenu
  
  // public
  property alias model: menuItemList.model
  property int popupOriginX: 0
  property int popupOriginY: 0
  property int fadeDuration: 300
  property bool ready: false
  
  // readonly
  property int cueArrowOffset: Math.floor(originCueWidth / 2)
  property int boundingMenuOffset: gu(0.5)
  
  signal actionInvoked(string name);
  signal popupInteractionCompleted();
  
  function flipVisibility() {
      state = (state == "shown" ? "hidden" : "shown");
  }
  
  x: {
    var result = (popupOriginX >= 0) ? popupOriginX - cueArrowOffset : (parent.width + popupOriginX) - width - cueArrowOffset;
    if (result < boundingMenuOffset)
      result = boundingMenuOffset;
    else if (result + width > parent.width - boundingMenuOffset)
      result = parent.width - width - boundingMenuOffset;
    
    return result;
  }
  y: {
    var result = (popupOriginY >= 0) ? popupOriginY : (parent.height + popupOriginY) - height;
    if (result < 0)
      result = boundingMenuOffset;
    else if (result + height > parent.height)
      result = parent.height - height - originCueHeight;
    
    return result;
  }
  width: gu(40)
  height: menuItemColumn.height + originCueHeight
  
  z: 32
  
  originCueX: {
    var result = (popupOriginX >= 0) ? x : width - originCueWidth;
    if (result < 0)
      result = 0;
    else if (result > width - originCueWidth)
      result = width - cueArrowOffset;
    
    return result;
  }
  
  state: "hidden"
  
  states: [
    State { name: "shown"; },
    State { name: "hidden"; }
  ]
  
  transitions: [
    Transition { from: "shown"; to: "hidden";
      FadeOutAnimation { target: popupMenu; duration: fadeDuration; }
    },
    Transition { from: "hidden"; to: "shown";
      FadeInAnimation { target: popupMenu; duration: fadeDuration; }
    }
  ]
  
  color: "transparent"
  
  Column {
    id: menuItemColumn
    
    Repeater {
      id: menuItemList
      
      x: popupMenu.contentLeft
      y: popupMenu.contentTop
      width: popupMenu.width
      
      delegate: MenuItem {
        title: (model.title) ? model.title : ""
        isSeparator: (model.isSeparator) ? model.isSeparator : false
        hasBottomBorder: (model.hasBottomBorder) ? model.hasBottomBorder : false
        iconFilename: (model.iconFilename) ? model.iconFilename : ""
        hasCueRectangle: (model.hasCueRectangle) ? model.hasCueRectangle : false
        action: (model.action) ? model.action : ""
        hostMenu: popupMenu
        
        onActionInvoked: {
          popupMenu.actionInvoked(name);
        }
        
        onPopupInteractionCompleted: {
          popupMenu.popupInteractionCompleted();
        }
      }
    }
  }
  
  Rectangle {
    id: overstroke
    
    width: parent.width
    height: parent.height - parent.originCueHeight
    
    color: "transparent"
    
    border.color: "#a7a9ac"
    border.width: 1
  }
}

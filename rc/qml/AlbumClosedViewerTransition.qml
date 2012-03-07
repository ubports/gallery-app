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

Item {
  id: albumClosedViewerTransition
  
  signal enterCompleted()
  
  property Album album
  property Rectangle backgroundGlass
  property int durationMsec: 1500
  
  function start(startRect) {
    albumPageViewer.x = startRect.x;
    albumPageViewer.y = startRect.y;
    albumPageViewer.width = startRect.width;
    albumPageViewer.height = startRect.height;
    
    albumPageViewer.turnFraction = 0.0;
    
    openAnimation.start();
  }
  
  AlbumPageViewer {
    id: albumPageViewer
    
    visible: false
    
    album: parent.album
    durationMsec: parent.durationMsec
    turnTowardPageNumber: (album) ? album.currentPageNumber : 0
  }
  
  SequentialAnimation {
    id: openAnimation
    
    PropertyAction {
      target: albumPageViewer
      property: "visible"
      value: true
    }
    
    PropertyAnimation {
      target: albumPageViewer
      property: "coverTitleOpacity"
      to: 0.0
      duration: 250
      easing.type: Easing.InOutQuad
    }
    
    ParallelAnimation {
      ExpandAnimation {
        target: albumPageViewer
        endHeight: albumClosedViewerTransition.height
        duration: durationMsec
        easingType: Easing.OutBack
        overshoot: 1.75
      }
      
      PropertyAnimation {
        target: albumPageViewer
        property: "turnFraction"
        from: 0.0
        to: 1.0
        duration: durationMsec / 3
        easing.type: Easing.InOutQuad
      }
      
      PropertyAnimation {
        target: backgroundGlass
        property: "opacity"
        to: 1.0
        duration: durationMsec / 2
        easing.type: Easing.OutQuad
      }
    }
    
    PropertyAction {
      target: albumPageViewer
      property: "visible"
      value: false
    }
    
    PropertyAction {
      target: backgroundGlass
      property: "opacity"
      value: 0.0
    }
    
    PropertyAction {
      target: album
      property: "closed"
      value: false
    }
    
    onCompleted: enterCompleted()
  }
}

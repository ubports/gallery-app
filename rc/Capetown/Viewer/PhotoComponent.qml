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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Eric Gregory <eric@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0

// Basic photo component.  Can be used on its own, or as a delegate
// for PhotoViewer.
Rectangle {
  id: photoComponent
  
  property alias source: image.source
  
  property bool isCropped: false
  property bool isPreview: false
  property bool isZoomable: false
  property bool isAnimate: false
  property string ownerName: "(not set)"
  
  // read-only
  property real paintedWidth: image.paintedWidth
  property real paintedHeight: image.paintedHeight
  property bool isLoaded: false
  
  signal loaded()
  
  clip: true

  Image {
    id: image

    width: parent.width
    height: parent.height
    x: 0
    y: 0

    // CRITICAL: when we moved to QT 5, we began to experience crashes when
    //           drawing photos after editing operations (see Launchpad bug
    //           #1065208). It turns out that, due to a bug in QT 5, in some
    //           cases images can be added to the list of objects to be drawn
    //           even though they're not yet loaded into memory. Of course,
    //           this causes a segfault. This property binding is here to
    //           prevent this segfault & crash from occurring.
	visible: isLoaded;
    
    // By using a minimum sourceSize width, can reduce the amount of I/O
    // fetching the same image
    // TODO: as we animate previews, since we're changing the sourceSize, it
    // triggers a reload from disk at each step.  We should cap these previews'
    // sourceSize so that stops happening.  (This might apply to more than
    // previews at some point in time.)
    sourceSize.width: (width <= 1024) ? 1024 : width
    
    asynchronous: !isAnimate
    cache: !isAnimate
    smooth: !isAnimate
    fillMode: isCropped ? Image.PreserveAspectCrop : Image.PreserveAspectFit
    
    onStatusChanged: {
      if(image.status == Image.Ready) {
        isLoaded = true;
        loaded();
      } else {
        isLoaded = false;
      }
    }
  }
}

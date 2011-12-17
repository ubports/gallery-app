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

import QtQuick 1.0

Rectangle {
  id: tablet_surface
  objectName: "tablet_surface"
  
  signal power_off()
  
  width: 1280
  height: 800
  color: "white"
  
  /*
   * There's a strong desire for each page to maintain its state while it's
   * in the background (not presented to the user), so when the user returns
   * they see the same content in the place they left it.  This precludes
   * using Loader, which destroys the current Component to load the new one.
   * A more robust solution may be to use a Repeater (or another list Component)
   * that dynamically adds pages from a model supplied by UIController.
   */
  
  Overview {
    id: overview
    objectName: "overview"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: true
  }
  
  PhotoViewer {
    id: photo_viewer
    objectName: "photo_viewer"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: false
  }
  
  AlbumViewer {
    id: album_viewer
    objectName: "album_viewer"
    
    x: 0
    y: 0
    width: parent.width
    height: parent.height
    
    visible: false
  }
}

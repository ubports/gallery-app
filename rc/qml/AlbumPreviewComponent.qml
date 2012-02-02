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
 */

import QtQuick 1.1
import Gallery 1.0

Rectangle {
  property AlbumPage albumPage
  
  width: 388
  height: 252
  
  AlbumPageComponent {
    id: albumPageComponent
    
    albumPage: parent.albumPage
    gutter: 8
    borderWidth: 1
    
    width: parent.width
    height: parent.height - text.height
  }
  
  Text {
    id: text
    
    height: 24
    
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: albumPageComponent.bottom
    anchors.topMargin: 12
    anchors.bottomMargin: 12
    
    color: "#657CA9"
    
    text: (albumPage) ? albumPage.owner.name : "";
    smooth: true
  }
  
  Image {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: albumPageComponent.top
    
    source: "../img/bookmark-ribbon.png"
    cache: true
  }
}

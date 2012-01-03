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

Rectangle {
  property string qmlRC
  property string albumName
  property variant mediaSourceList
  
  width: 388
  height: 252
  
  onMediaSourceListChanged: {
    if (!loader.item)
      return;
    
    // if the preview_list changed, force the loader to reload the same
    // qml_rc (which is then populated with the new previews)
    var src = loader.source;
    loader.source = "";
    loader.source = src;
  }
  
  Loader {
    id: loader
    
    width: parent.width
    height: parent.height
    clip: true
    
    source: qmlRC
    
    onLoaded: {
      if (item) {
        item.mediaSourceList = mediaSourceList;
        item.width = parent.width;
        item.height = parent.height;
        item.gutter = 8;
      }
    }
  }
  
  Text {
    height: 24
    
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: loader.bottom
    anchors.topMargin: 12
    anchors.bottomMargin: 12
    
    color: "#657CA9"
    
    text: albumName
    smooth: true
  }
}
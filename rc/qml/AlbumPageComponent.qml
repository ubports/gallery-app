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

Rectangle {
  id: albumPageComponent
  
  property AlbumPage albumPage: null
  property int gutter: 24
  property int photoBorderWidth: 0
  property color photoBorderColor: "#95b5de"
  property bool isPreview: false
  
  onAlbumPageChanged: {
    // force reload of the entire page's QML
    loader.source = (albumPage) ? albumPage.qmlRC : "";
  }
    
  AlbumCover {
    id: albumCover
    
    album: (albumPage) ? albumPage.owner : null
    
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.horizontalCenter
    anchors.right: parent.right
    
    width: parent.width / 2
    
    visible: (album != null)
  }
  
  Loader {
    id: loader
    
    // read-only
    property variant mediaSourceList: (albumPage) ? albumPage.mediaSourceList : null
    property alias gutter: albumPageComponent.gutter
    property alias photoBorderWidth: albumPageComponent.photoBorderWidth
    property alias photoBorderColor: albumPageComponent.photoBorderColor
    
    anchors.fill: parent
    
    onMediaSourceListChanged: {
      if (!mediaSourceList || source == "")
        return;
      
      // MediaSources within page have changed, force reload of same QML file
      var src = source;
      source = "";
      source = src;
    }
    
    source: (albumPage) ? albumPage.qmlRC : ""
    
    visible: (source != "")
    
    onLoaded: {
      item.mediaSourceList = albumPage.mediaSourceList;
      item.width = albumPageComponent.width;
      item.height = albumPageComponent.height;
      item.isPreview = albumPageComponent.isPreview
    }
  }
}

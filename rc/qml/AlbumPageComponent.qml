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
  
  property AlbumPage albumPage

  property real topGutter: gu(0)
  property real bottomGutter: gu(0)
  property real leftGutter: gu(3)
  property real rightGutter: gu(3)
  property real spineGutter: gu(2)
  property real insideGutter: gu(2)

  property bool isPreview: false

  property real spineBorderWidth: gu(0.25)
  property color borderColor: (isPreview ? "#0072bc" : "#95b5df")
  
  property alias bookmarkOpacity: bookmark.opacity

  border.width: (isPreview ? gu(0.25) : gu(0))
  border.color: borderColor

  onAlbumPageChanged: {
    // force reload of the entire page's QML
    loader.source = (albumPage) ? albumPage.qmlRC : "";
  }
  
  Loader {
    id: loader
    
    // read-only
    property variant mediaSourceList: (albumPage) ? albumPage.mediaSourceList : null
    property alias topGutter: albumPageComponent.topGutter
    property alias bottomGutter: albumPageComponent.bottomGutter
    property alias leftGutter: albumPageComponent.leftGutter
    property alias rightGutter: albumPageComponent.rightGutter
    property alias spineGutter: albumPageComponent.spineGutter
    property alias insideGutter: albumPageComponent.insideGutter
    property alias spineBorderWidth: albumPageComponent.spineBorderWidth
    property alias spineBorderColor: albumPageComponent.borderColor

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

  Image {
    id: bookmark

    // internal
    property real canonicalPreviewWidth: gu(50)

    visible: albumPageComponent.isPreview

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top

    transformOrigin: Item.Top
    // Scale the bookmark accordingly if the preview has been resized.
    scale: parent.width / canonicalPreviewWidth

    source: "../img/bookmark-ribbon.png"
    cache: true
  }
}

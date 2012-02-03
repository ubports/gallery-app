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

Loader {
  id: loader
  
  property AlbumPage albumPage
  property int gutter: 24
  property int borderWidth: 1
  
  // read-only
  property variant mediaSourceList: (albumPage) ? albumPage.mediaSourceList : null
  
  onAlbumPageChanged: {
    // force reload of the entire page's QML
    source = (albumPage) ? albumPage.qmlRC : "";
  }
  
  onMediaSourceListChanged: {
    if (!mediaSourceList || source == "")
      return;
    
    // MediaSources within page have changed, force reload of same QML file
    var src = source;
    source = "";
    source = src;
  }
  
  source: (albumPage) ? albumPage.qmlRC : ""
  
  onLoaded: {
    item.mediaSourceList = albumPage.mediaSourceList;
    item.width = loader.width;
    item.height = loader.height;
    item.gutter = loader.gutter
    item.borderWidth = loader.borderWidth
  }
}

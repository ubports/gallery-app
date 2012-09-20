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

Checkerboard {
  id: eventCheckerboard
  
  // Set this to an album to "grey out" photos already in the album.
  property variant disableMediaInAlbum: null
  
  property real selectionFooterHeight: gu(6) // Toolbar height

  function getVisibleMediaSources() {
    var vd = getVisibleDelegates();
    
    // return the objects, not the items, to the caller
    var vo = [];
    for (var ctr = 0; ctr < vd.length; ctr++) {
      var item = vd[ctr];
      
      if (!item.event)
        vo[vo.length] = item.mediaSource;
    }
    
    return vo;
  }
  
  function getVisibleEvents() {
    var vd = getVisibleDelegates();
    
    // return objects, not the items, to caller
    var ve = [];
    for (var ctr = 0; ctr < vd.length; ctr++) {
      var item = vd[ctr];
      
      if (item.event)
        ve[ve.length] = item.event;
    }
    
    return ve;
  }
  
  function getRectOfObject(object) {
    var index = model.indexOf(object);
    
    return (index >= 0) ? getRectOfItemAt(index, eventCheckerboard, false) : undefined;
  }
  
  model: EventOverviewModel {
    monitored: true
  }
  
  delegate: CheckerboardDelegate {
    id: eventCheckerboardDelegate
    
    checkerboard: eventCheckerboard
    enabled: true
    
    // internal
    // Needed for getVisibleMediaSources() and getVisibleEvents() above.
    property alias mediaSource: preview.mediaSource
    property alias event: preview.event
    
    // If an album is specified, disable photos already in the album.  If this
    // delegate is an event card, disable the event card if all the photos are
    // present in that album.
    function updateEnabled() {
      if (!checkerboard.disableMediaInAlbum)
        return;
      
      if (mediaSource) {
        eventCheckerboardDelegate.enabled =
          !checkerboard.disableMediaInAlbum.containsMedia(modelData.mediaSource);
      } else {
        eventCheckerboardDelegate.enabled = !disableMediaInAlbum.containsAll(event);
      }
    }
    
    Connections {
      target: checkerboard
      ignoreUnknownSignals: true
      
      onDisableMediaInAlbumChanged: updateEnabled()
    }
    
    Connections {
      target: checkerboard.disableMediaInAlbum || null
      ignoreUnknownSignals: true
      
      onAllMediaSourcesChanged: updateEnabled()
    }
    
    Component.onCompleted: updateEnabled()
    
    content: EventCheckerboardPreview {
      id: preview

      ownerName: "EventCheckerboard"

      mediaSource: (modelData.typeName == "MediaSource") ? modelData.mediaSource : undefined
      event: (modelData.typeName == "Event") ? modelData.object : undefined
      isSelected: eventCheckerboardDelegate.isSelected && eventCheckerboardDelegate.enabled
    }
  }

  footer: Item {
    id: selectionSpacer

    width: parent.width
    height: (inSelectionMode ? selectionFooterHeight : 0)

    onHeightChanged: returnToBounds()
  }
}

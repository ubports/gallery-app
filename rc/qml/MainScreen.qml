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
 * Charles Lindsay <chaz@yorba.org
 */

import QtQuick 2.0
import "Utility"

Item {
  id: mainScreen

  Component.onCompleted: navStack.switchToPage(overview)

  TabView {
    id: overview

    anchors.fill: parent

    visible: false

    onAlbumSelected: {
      albumViewerLoader.load();
      
      if (thumbnailRect) {
        showAlbumPreview(album, false);
        albumViewerTransition.transitionToAlbumViewer(album, thumbnailRect);
      } else {
        albumViewerLoader.item.resetView(album);
        albumViewerTransition.dissolve(overview, albumViewerLoader);
      }
    }
  }

  Loader {
    id: albumViewerLoader
    
    anchors.fill: parent
    
    visible: false
    
    function load() {
      if (!sourceComponent)
        sourceComponent = albumViewerComponent
    }
    
    Component {
      id: albumViewerComponent
      
      AlbumViewer {
        id: albumViewer
        
        onCloseRequested: {
          if (!album) {
            // TODO: this isn't quite right.  Not sure how this should look.
            albumViewerLoader.visible = false;
            albumViewerTransition.dissolve(null, navStack.previous());
          } else if (state == "gridView") {
            albumViewerTransition.dissolve(albumViewerLoader, navStack.previous());
          } else {
            navStack.goBack();
            
            var thumbnailRect = overview.getRectOfAlbumPreview(album, albumViewerTransition);
            if (thumbnailRect) {
              overview.showAlbumPreview(album, false);
              albumViewerTransition.transitionFromAlbumViewer(
                  album, thumbnailRect, stayOpen, viewingPage);
            }
          }
        }
      }
    }
  }
  
  AlbumViewerTransition {
    id: albumViewerTransition

    anchors.fill: albumViewerLoader

    backgroundGlass: overview.glass
    isPortrait: application.isPortrait

    onTransitionToAlbumViewerCompleted: {
      navStack.switchToAlbumViewer(album);
      overview.showAlbumPreview(album, true);
    }

    onTransitionFromAlbumViewerCompleted: overview.showAlbumPreview(album, true)

    onDissolveCompleted: {
      if (fadeInTarget == navStack.previous())
        navStack.goBack();
      else
        navStack.switchToPage(fadeInTarget);
    }
  }
  
  NavStack {
    id: navStack

    function switchToAlbumViewer(album) {
      albumViewerLoader.load();
      albumViewerLoader.item.resetView(album);

      navStack.switchToPage(albumViewerLoader);
    }
  }

  MouseArea {
    id: transitionClickBlocker

    anchors.fill: parent

    visible: albumViewerTransition.animationRunning
      || (albumViewerLoader.item && albumViewerLoader.item.animationRunning)
  }
}

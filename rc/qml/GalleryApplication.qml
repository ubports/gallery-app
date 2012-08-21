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
import Gallery 1.0
import "GalleryUtility.js" as GalleryUtility

Item {
  id: application
  
  // readonly
  property bool isPortrait: (height > width)

  // Converts a grid-unit into a pixel value.
  function gu(amt) {
    return Math.floor(GRIDUNIT * amt);
  }

  // gu(), but for font point sizes.
  function pointUnits(amt) {
    // 1.6 is a hacky fudge-factor for now.
    return Math.round(1.6 * amt);
  }
  
  width: gu(160)
  height: gu(100)

  Component.onCompleted: navStack.switchToPage(overview)
  
  Overview {
    id: overview
    
    anchors.fill: parent
    
    visible: false

    onAlbumSelected: {
      if (thumbnailRect) {
        showAlbumPreview(album, false);
        albumViewerTransition.transitionToAlbumViewer(album, thumbnailRect);
      } else {
        albumViewer.resetView(album);
        albumViewerTransition.dissolve(overview, albumViewer);
      }
    }

    onEditPhotoRequested: navStack.switchToPhotoEditor(photo)
  }

  AlbumViewer {
    id: albumViewer
    
    anchors.fill: parent
    
    visible: false
    
    onCloseRequested: {
      if (!album) {
        // TODO: this isn't quite right.  Not sure how this should look.
        albumViewer.visible = false;
        albumViewerTransition.dissolve(null, navStack.previous());
      } else if (state == "gridView") {
        albumViewerTransition.dissolve(albumViewer, navStack.previous());
      } else {
        navStack.goBack();

        var thumbnailRect = overview.getRectOfAlbumPreview(album, albumViewerTransition);
        if (thumbnailRect) {
          overview.showAlbumPreview(album, false);
          albumViewerTransition.transitionFromAlbumViewer(album, thumbnailRect, stayOpen);
        }
      }
    }

    onEditPhotoRequested: navStack.switchToPhotoEditor(photo)
  }

  AlbumViewerTransition {
    id: albumViewerTransition

    anchors.fill: albumViewer

    backgroundGlass: overview.glass

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
  
  PhotoEditor {
    id: photoEditor

    z: 20
    anchors.fill: parent

    visible: false

    onCloseRequested: navStack.goBack()
  }

  NavStack {
    id: navStack
    
    function switchToPhotoEditor(photo) {
      photoEditor.enterEditor(photo);

      navStack.switchToPage(photoEditor);
    }

    function switchToAlbumViewer(album) {
      albumViewer.resetView(album);
      
      navStack.switchToPage(albumViewer);
    }
  }
}

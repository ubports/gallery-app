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
          albumViewerTransition.transitionFromAlbumViewer(
              album, thumbnailRect, stayOpen, viewingPage);
        }
      }
    }
  }

  AlbumViewerTransition {
    id: albumViewerTransition

    anchors.fill: albumViewer

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
      albumViewer.resetView(album);

      navStack.switchToPage(albumViewer);
    }
  }

  MouseArea {
    id: transitionClickBlocker

    anchors.fill: parent

    visible: albumViewerTransition.animationRunning || albumViewer.animationRunning
  }
}

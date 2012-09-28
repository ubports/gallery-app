/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import "../Capetown"
import "../js/GalleryUtility.js" as GalleryUtility
import "../js/GraphicsRoutines.js" as GraphicsRoutines
import "Components"
import "Widgets"

Item {
  id: albumEditor

  signal closeRequested(variant album, bool enterViewer)

  property Album album
  property real minimumCoverWidth: gu(32)
  property real minimumCoverHeight: gu(38)
  property real preferredCoverWidth: width - gu(8)
  property real preferredCoverHeight: height - gu(8)
  property real minimumTopMargin: gu(3)

  // readonly
  property variant editorRect
  property alias animationRunning: mediaSelector.animationRunning

  // internal
  property real canonicalWidth: gu(66)
  property real canonicalHeight: gu(80)

  function editNewAlbum() {
    albumEditor.album = albumModel.createOrphan();
    coverMenu.state = "hidden"
  }

  function editAlbum(album) {
    albumEditor.album = album;
    coverMenu.state = "hidden"
  }

  // internal
  function closeAlbum() {
    if (album.populatedContentPageCount > 0) {
      albumModel.addOrphan(album);

      // Don't want to stay on the cover.
      if (album.currentPage == album.firstValidCurrentPage)
        album.currentPage = album.firstContentPage;
    } else {
      albumModel.destroyOrphan(album);
    }
  }

  // internal
  function resetEditorRect() {
    editorRect = GalleryUtility.getRectRelativeTo(cover, albumEditor);
  }

  onAlbumChanged: resetEditorRect() // HACK: works, but not conceptually correct.
  onWidthChanged: resetEditorRect()
  onHeightChanged: resetEditorRect()

  AlbumCollectionModel {
    id: albumModel
  }
  
  MouseArea {
    id: coverCloser
    
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    anchors.fill: parent
    onPressed: {
      coverMenu.state = "hidden";
      cover.editingDone();
      closeAlbum();
      
      albumEditor.closeRequested(albumEditor.album, false);
    }
  }

  AspectArea {
    id: coverArea

    x: (parent.width - width) / 2
    y: Math.max((parent.height - height) / 2, minimumTopMargin)

    width: GraphicsRoutines.clamp(
        preferredCoverWidth, minimumCoverWidth, canonicalWidth)
    height: GraphicsRoutines.clamp(
        preferredCoverHeight, minimumCoverHeight, canonicalHeight)

    aspectWidth: canonicalWidth
    aspectHeight: canonicalHeight
    
    content: AlbumCover {
      id: cover

      anchors.fill: parent

      album: albumEditor.album
      isPreview: false
      
      onPressed: {
        mouse.accepted = true;
        if (!isTextEditing) {
          coverMenu.flipVisibility();
        } else {
          cover.editingDone()
        }
      }
      
      onIsTextEditingChanged: {
        // Hide menu when we start editing text.
        if (isTextEditing && coverMenu.state !== "hidden")
          coverMenu.state = "hidden";
      }

      MouseArea {
        id: addPhotosButton
        
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: gu(14)
        height: gu(14)
        onClicked: mediaSelector.show()
      }
    }
  }
  
  // Cover picker
  AlbumCoverMenu {
    id: coverMenu
    
    visible: false
    state: "hidden"
    popupOriginX: -gu(3)
    popupOriginY: -gu(15)
    
    onActionInvoked: {
      albumEditor.album.coverNickname = name
      state = "hidden"
    }
  }
  
  MediaSelector {
    id: mediaSelector

    anchors.fill: parent

    album: albumEditor.album

    onCancelRequested: hide()

    onDoneRequested: {
      album.addSelectedMediaSources(model);
      closeAlbum();

      albumEditor.closeRequested(albumEditor.album, true);
      hide();
    }
  }
}

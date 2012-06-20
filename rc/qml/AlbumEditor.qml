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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0
import "GalleryUtility.js" as GalleryUtility

Item {
  id: albumEditor

  signal closeRequested(variant album)
  signal addPhotosRequested(variant album)

  property Album album

  // readonly
  property alias editorX: cover.x
  property alias editorY: cover.y
  property alias editorWidth: cover.width
  property alias editorHeight: cover.height

  function editNewAlbum() {
    albumEditor.album = albumModel.createOrphan();
    coverMenu.state = "hidden"
  }

  function editAlbum(album) {
    albumEditor.album = album;
    coverMenu.state = "hidden"
  }

  AlbumCollectionModel {
    id: albumModel
  }

  MouseArea {
    id: blocker

    anchors.fill: parent
  }

  AlbumCover {
    id: cover

    anchors.centerIn: parent
    width: gu(66)
    height: gu(80)

    album: albumEditor.album
    isPreview: false
    
    MouseArea {
      anchors.fill: parent
      
      onPressed: coverMenu.flipVisibility()
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

  Image {
    anchors.verticalCenter: cover.top
    anchors.horizontalCenter: cover.right

    source: "../img/album-edit-close.png"
    cache: true

    MouseArea {
      anchors.fill: parent
      onClicked: {
        if (albumEditor.album.contentPageCount > 0) {
          albumModel.addOrphan(albumEditor.album);
        } else {
          albumModel.destroyOrphan(albumEditor.album);
          albumEditor.album = null;
        }

        albumEditor.closeRequested(albumEditor.album);
      }
    }
  }

  MouseArea {
    anchors.right: cover.right
    anchors.bottom: cover.bottom
    width: gu(14)
    height: gu(14)

    onClicked: albumEditor.addPhotosRequested(albumEditor.album)
  }
}

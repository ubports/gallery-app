/*
 * Copyright (C) 2012-2015 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Gallery 1.0

/*!
  Popover dialog to delete an album.
  Don't forget to set the album property
*/
Popover {
    /// The album that is to be deleted
    property variant album: null

    /// The delete album button was clicked
    signal deleteClicked()
    /// The delete album + contntents was clicked
    signal deleteWithContentsClicked()

    /// Used internally to delete the album
    property AlbumCollectionModel __albumCollection: AlbumCollectionModel {}
    /// Used internally to delete the content of the album
    property MediaCollectionModel __mediaCollection: MediaCollectionModel {}

    Column {
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        ListItem.Standard {
            text: i18n.tr("Delete album")
            selected: false
            onClicked: {
                deleteClicked()
                hide()
                __albumCollection.destroyAlbum(album)
            }
        }
        ListItem.Standard {
            text: i18n.tr("Delete album AND contents")
            selected: false
            onClicked: {
                deleteWithContentsClicked()
                hide()

                // Remove contents.
                var list = album.allMediaSources;
                for (var i = list.length-1; i >= 0; i--)
                    __mediaCollection.destroyMedia(list[i], true);

                // Remove album.
                __albumCollection.destroyAlbum(album);
            }
        }
    }
}

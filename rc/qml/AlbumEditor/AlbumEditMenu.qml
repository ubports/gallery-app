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
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem

/*!
  Popover to show the context menu for an album
*/
Popover {
    id: albumEditMenu

    /// The user clicked on the edit button
    signal editClicked()
    /// The user clicked on the delete button
    signal deleteClicked()

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        ListItem.Standard {
            objectName: "editAlbumListItem"
            text: i18n.tr("Edit album")
            onClicked: {
                hide()
                editClicked()
            }
        }
        ListItem.Standard {
            text: i18n.tr("Delete")
            onClicked: {
                hide()
                deleteClicked()
            }
        }
    }
}

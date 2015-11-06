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
import "../AlbumViewer"

/*!
  Menu to select the album cover
*/
Popover {
    id: albumCoverMenu

    signal newCoverSelected(string coverName)

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Repeater {
            model: AlbumCoverList {}

            delegate: ListItem.Standard {
                objectName: "albumCoverMenuItem_" + coverName
                text: title
                icon: Qt.resolvedUrl(iconFilename)
                onClicked: {
                    albumCoverMenu.newCoverSelected(coverName);
                    hide();
                }
            }
        }
    }
}

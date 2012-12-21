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
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Popover {
    id: deletePopover

    signal deleteClicked

    height: units.gu(6)

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: units.gu(6)

        ListItem.Empty {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: units.gu(6)

            Row {
                anchors {
                    fill: parent
                    leftMargin: units.gu(2)
                    rightMargin: units.gu(2)
                    topMargin: units.gu(1)
                    bottomMargin: units.gu(1)
                }
                spacing: units.gu(2)

                Button {
                    height: units.gu(4)
                    width: units.gu(17)
                    color: "grey"
                    text: "Cancel"
                    onClicked: deletePopover.hide()
                }

                Button {
                    height: units.gu(4)
                    width: units.gu(17)
                    color: "#c94212"
                    text: "Delete"
                    onClicked: deletePopover.deleteClicked()
                }
            }
        }
    }
}

// TODO: Re-use this code when the albums are functional again
//        ListItem.Standard {
//            text: "Remove from album"
//            onClicked: {
//                album.removeMediaSource(photo);
//                deletePopover.finishRemove();
//                deletePopover.hide();
//            }
//            visible: (album !== undefined)
//        }

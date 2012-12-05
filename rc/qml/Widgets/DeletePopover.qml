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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Popover {
    id: deletePopover

    property var model
    property var album
    property var photo
    property var photoViewer

    // internal
    function finishRemove() {
        if (!album === undefined) return;
        if (model.count === 0) photoViewer.closeRequested();
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        ListItem.Standard {
            text: "Delete photo"
            onClicked: {
                model.destroyMedia(photo);
                deletePopover.finishRemove();
                deletePopover.hide();
            }
        }

        ListItem.Standard {
            text: "Remove from album"
            onClicked: {
                album.removeMediaSource(photo);
                deletePopover.finishRemove();
                deletePopover.hide();
            }
            visible: (album !== undefined)
        }
    }
}

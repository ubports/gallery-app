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
import ".."

Popover {
    property variant photo: null
    property CropInteractor cropper

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        ListItem.Standard {
            text: "Rotate"
            objectName: "rotateListItem"
            onClicked: {
                hide();
                photo.rotateRight();
            }
        }
        ListItem.Standard {
            text: "Crop"
            objectName: "cropListItem"
            onClicked: {
                hide();
                cropper.show(photo);
            }
        }
        ListItem.Standard {
            text: "Auto enhance"
            objectName: "enhanceListItem"
            onClicked: {
                hide();
                photo.autoEnhance();
            }
        }
        ListItem.Standard {
            text: "Undo"
            objectName: "undoListItem"
            onClicked: {
                hide();
                photo.undo();
            }
        }
        ListItem.Standard {
            text: "Redo"
            objectName: "redoListItem"
            onClicked: {
                hide();
                photo.redo();
            }
        }
        ListItem.Standard {
            text: "Revert to original"
            objectName: "revertListItem"
            onClicked: {
                hide();
                photo.revertToOriginal();
            }
        }
    }
}

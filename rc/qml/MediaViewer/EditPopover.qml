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

/*!
*/
Popover {
    id: root
    /*!
    */
    property variant photo: null
    /*!
    */
    property CropInteractor cropper

    ///
    signal buttonPressed()

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        ListItem.Standard {
            text: i18n.tr("Rotate")
            objectName: "rotateListItem"
            onClicked: {
                hide();
                photo.rotateRight();
                root.buttonPressed();
            }
        }
        ListItem.Standard {
            text: i18n.tr("Crop")
            objectName: "cropListItem"
            onClicked: {
                hide();
                cropper.show(photo);
                root.buttonPressed();
            }
        }
        ListItem.Standard {
            text: i18n.tr("Auto enhance")
            objectName: "enhanceListItem"            
            onClicked: {
                hide();
                photo.autoEnhance();
                root.buttonPressed();
            }
        }
        ListItem.Standard {
            text: i18n.tr("Undo")
            objectName: "undoListItem"
            enabled: photo.canUndo
            onClicked: {
                hide();
                photo.undo();
                root.buttonPressed();
            }
        }
        ListItem.Standard {
            text: i18n.tr("Redo")
            objectName: "redoListItem"
            enabled: photo.canRedo
            onClicked: {
                hide();
                photo.redo();
                root.buttonPressed();
            }
        }
        ListItem.Standard {
            text: i18n.tr("Revert to original")
            objectName: "revertListItem"
            enabled: !photo.isOriginal
            onClicked: {
                hide();
                photo.revertToOriginal();
                root.buttonPressed();
            }
        }
    }
}

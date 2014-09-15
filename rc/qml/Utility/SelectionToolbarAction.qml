/*
 * Copyright (C) 2013 Canonical Ltd
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

/*!
  The actions for the toolbar for the event and photos view in select mode
  */
ToolbarItems {
    id: root
    /// The selection state item
    property SelectionState selection: null

    /// Emitted when cancel was clicked
    signal cancelClicked()
    /// Emitted when add was clicked
    /// @param caller the item that caused the call (was clicked)
    signal addClicked(var caller)
    ///Emitted when delete was clicked
    signal deleteClicked()
    ///Emitted when share was clicked
    signal shareClicked()

    // in selection mode, never hide the toolbar:
    opened: true
    locked: true

    ToolbarButton {
        id: addButton
        objectName: "addButton"
        action: Action {
            text: i18n.tr("Add")
            iconName: "add"
            enabled: root.selection.selectedCount > 0
            onTriggered: root.addClicked(addButton);
        }
    }
    ToolbarButton {
        action: Action {
            objectName: "deleteButton"
            text: i18n.tr("Delete")
            iconName: "delete"
            enabled: root.selection.selectedCount > 0
            onTriggered:root.deleteClicked();
        }
    }
    ToolbarButton {
        action: Action {
            objectName: "shareButton"
            text: i18n.tr("Share")
            iconName: "share"
            enabled: root.selection.selectedMediaCount == 1
            onTriggered: root.shareClicked();
        }
    }

    back:  Button {
        objectName: "cancelButton"
        action: Action {
            text: i18n.tr("Cancel")
            iconName: "back"
            onTriggered: {
                root.cancelClicked();
            }
        }
    }
}

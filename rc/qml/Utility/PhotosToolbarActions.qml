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

/*!
  The actions for the toolbar for the event and photos view
  */
ToolbarItems {
    id: root
    /// The selection state item to use for enable / disable the select mode
    property SelectionState selection: null

    /// Emittedt when the camera action was triggered
    signal startCamera()

    ToolbarButton {
        action: Action {
            objectName: "selectButton"
            text: i18n.tr("Select")
            iconSource: Qt.resolvedUrl("../../img/select.png")
            enabled: root.selection !== null
            onTriggered: root.selection.inSelectionMode = true;
        }
    }
    ToolbarButton {
        objectName: "importButton"
        action: Action {
            text: i18n.tr("Import")
            iconSource: Qt.resolvedUrl("../../img/import-image.png")
            visible: false
        }
    }
    ToolbarButton {
        action: Action {
            objectName: "cameraButton"
            text: i18n.tr("Camera")
            visible: !APP.desktopMode
            iconSource: Qt.resolvedUrl("../../img/camera.png")
            onTriggered: Qt.openUrlExternally("appid://com.ubuntu.camera/camera/current-user-version")
        }
    }
}

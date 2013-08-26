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

/*!
  Wrapper to call ApplicationManager functions. To call other apps.
  On platforms where the ApplicationManager is not available, warnings will be printed.
*/
Item {
    /// Opens the camera applications
    function switchToCameraApplication() {
        if (appManager.status === Loader.Ready) {
            appManager.item.switchToCameraApplication();
        } else {
            console.log("Switching applications is not supported on this platform.");
        }
    }

    Loader {
        id: appManager
        source: Qt.resolvedUrl("UbuntuApplicationWrapper.qml")
    }
}

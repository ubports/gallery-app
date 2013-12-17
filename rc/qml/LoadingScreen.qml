/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org
 */

import QtQuick 2.0
import Ubuntu.Components 0.1

/*!
*/
Item {
    id: loadingScreen

    Image {
        anchors.fill: parent
        source: "../img/background-paper.png"
        fillMode: Image.Tile
    }

    Item {
        anchors.centerIn: parent

        ActivityIndicator {
            id: loading
            anchors.verticalCenter: parent.verticalCenter
            running: true
        }

        Label {
            anchors.top: loading.bottom
            text: i18n.tr("Loading…")
            anchors.horizontalCenter: parent.horizontalCenter
            fontSize: "large"
        }
    }
}

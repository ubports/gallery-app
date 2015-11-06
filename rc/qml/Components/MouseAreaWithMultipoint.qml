/*
 * Copyright (C) 2014-2015 Canonical Ltd
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

import QtQuick 2.4
Item {
    id: mouseAreaWithMultipoint

    signal clicked(var mouse)
    signal doubleClicked(var mouse)
    signal wheel(var wheel)
    signal pressed(var mouse)

    property bool desktop: true
    property bool propagateComposedEvents: false

    anchors.fill: parent

    MouseArea {
        anchors.fill: parent
        enabled: mouseAreaWithMultipoint.desktop

        propagateComposedEvents: mouseAreaWithMultipoint.propagateComposedEvents

        onClicked: mouseAreaWithMultipoint.clicked(mouse);
        onDoubleClicked: mouseAreaWithMultipoint.doubleClicked(mouse);
        onWheel: mouseAreaWithMultipoint.wheel(wheel);
        onPressed: mouseAreaWithMultipoint.pressed(mouse);
    }

    MultiPointTouchArea {
        anchors.fill: parent
        enabled: !mouseAreaWithMultipoint.desktop

        Timer {
            id: dblTapTimeout
            interval: 600
            repeat: false
            onTriggered: dblTapHandler.pressCount = 0
        }

        touchPoints: TouchPoint {
            id: dblTapHandler
            property int pressCount: 0
            onPressedChanged: {
                if (pressed) {
                    pressCount++;
                    dblTapTimeout.running = true;
                } else {
                    if (pressCount == 1) {
                        mouseAreaWithMultipoint.clicked({ x: dblTapHandler.x, y: dblTapHandler.y });
                    } else if (pressCount === 2) {
                        mouseAreaWithMultipoint.doubleClicked({ x: dblTapHandler.x, y: dblTapHandler.y });
                        pressCount = 0;
                        dblTapTimeout.stop();
                    }
                }
            }
        }
    }
}

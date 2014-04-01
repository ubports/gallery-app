/*
 * Copyright (C) 2014 Canonical Ltd
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

MultiPointTouchArea {
    signal clicked(var mouse)
    signal doubleClicked(var mouse)

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
                    clicked({ x: dblTapHandler.x, y: dblTapHandler.y });
                } else if (pressCount === 2) {
                    doubleClicked({ x: dblTapHandler.x, y: dblTapHandler.y });
                    pressCount = 0;
                    dblTapTimeout.stop();
                }
            }
        }
    }
}

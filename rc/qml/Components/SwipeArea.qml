/*
 * Copyright (C) 2012-2015 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

// Because some QML components' natural swiping motion requires custom
// event handling, this emulates it and allows for programmatic control
// when a swipe has occurred.
//
// TODO: Currently no support for vertical swiping
MouseArea {
    /*!
    */
    signal startSwipe(bool leftToRight, int start)
    /*!
    */
    signal swiping(bool leftToRight, int start, int distance)
    /*!
    */
    signal swiped(bool leftToRight)
    /*!
    */
    signal tapped(int x, int y, bool rightButton)
    /*!
    */
    signal longPressed(int x, int y)

    /*!
     */
    property int requiredHorizMovement: units.gu(1)

    // internal
    /*!
    */
    property int startX: -1
    /*!
    */
    property bool leftToRight: true
    /*!
    */
    property bool swipeStarted: false
    /*!
    */
    property bool longPress: false

    // internal
    /*!
    */
    function reset() {
        startX = -1;
        leftToRight = true;
        swipeStarted = false;
        longPress = false;
    }

    preventStealing: swipeStarted
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onEnabledChanged: reset()
    onVisibleChanged: reset()

    onPositionChanged: {
        if (longPress)
            return;

        // look for initial swipe
        if (startX == -1) {
            startX = mouse.x;

            return;
        }

        var diff = 0;
        if (mouse.x < startX) {
            // once started, don't signal anything on other side of bounding point
            if (swipeStarted && leftToRight)
                return;

            diff = startX - mouse.x;
            leftToRight = false;
        } else if (mouse.x > startX) {
            // once started, don't signal anything on other side of bounding point
            if (swipeStarted && !leftToRight)
                return;

            diff = mouse.x - startX;
            leftToRight = true;
        } else {
            // If mouse.x == startX, there's no horizontal swiping going on.  This
            // lets e.g. the GridView steal vertical mouse movement for its flicking.
            return;
        }

        if (swipeStarted) {
            swiping(leftToRight, startX, diff);

            return;
        }

        if (diff < requiredHorizMovement)
            return;

        swipeStarted = true;

        startSwipe(leftToRight, startX);
        swiping(leftToRight, startX, diff);
    }

    onPressAndHold: {
        if (swipeStarted)
            return;

        longPress = true;
        longPressed(mouse.x, mouse.y);
    }

    onReleased: {
        if (swipeStarted)
            swiped(leftToRight);
        else if (!longPress)
            tapped(mouse.x, mouse.y, (mouse.button == Qt.RightButton));

        reset();
    }
}

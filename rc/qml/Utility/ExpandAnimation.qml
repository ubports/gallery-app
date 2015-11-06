/*
 * Copyright (C) 2011-2015 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4

// Moves the position and size of the target to the end values.
/*!
*/
ParallelAnimation {
    id: expandAnimation
    objectName: "expandAnimation"

    /*!
    */
    property variant target

    /*!
    */
    property real endX: 0
    /*!
    */
    property real endY: 0
    /*!
    */
    property real endWidth: target.parent.width
    /*!
    */
    property real endHeight: target.parent.height
    /*!
    */
    property int easingType: Easing.InQuad

    /*!
    */
    property int duration: 200

    NumberAnimation {
        target: expandAnimation.target
        property: "x"
        to: endX
        duration: expandAnimation.duration
        easing.type: easingType
    }

    NumberAnimation {
        target: expandAnimation.target
        property: "y"
        to: endY
        duration: expandAnimation.duration
        easing.type: easingType
    }

    NumberAnimation {
        target: expandAnimation.target
        property: "width"
        to: endWidth
        duration: expandAnimation.duration
        easing.type: easingType
    }

    NumberAnimation {
        target: expandAnimation.target
        property: "height"
        to: endHeight
        duration: expandAnimation.duration
        easing.type: easingType
    }
}

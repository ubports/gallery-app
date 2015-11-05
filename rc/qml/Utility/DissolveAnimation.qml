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

// Fades in the fadeInTarget while fading out the fadeOutTarget.
/*!
*/
SequentialAnimation {
    id: dissolveAnimation

    /*!
    */
    property Item fadeInTarget
    /*!
    */
    property Item fadeOutTarget
    /*!
    */
    property int duration: 200
    /*!
    */
    property int easingType: Easing.InQuad

    ParallelAnimation {
        FadeInAnimation {
            target: fadeInTarget
            duration: dissolveAnimation.duration
            easingType: dissolveAnimation.easingType
        }

        FadeOutAnimation {
            target: fadeOutTarget
            duration: dissolveAnimation.duration
            easingType: dissolveAnimation.easingType
        }
    }
}

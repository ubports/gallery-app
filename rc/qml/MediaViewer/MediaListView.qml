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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import "../Components"

// Displays a flickable photo stream.
//
// When implementing this, override onCurrentIndexChanged to load the 
// appropriate photo for the index.
ListView {
    id: mediaListView

    /*!
    */
    property int currentIndexForHighlight: -1

    // NOTE: These properties should be treated as read-only, as setting them
    // individually can lead to bogus results.  Use setCurrentIndex() to
    // initialize the view.
    property alias index: mediaListView.currentIndex

    /*!
    */
    function setCurrentIndex(index) {
        if (currentIndex === index)
            return;

        currentIndex = index;
        positionViewAtIndex(currentIndex, ListView.Beginning);
    }

    spacing: units.gu(5)
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    highlightRangeMode: ListView.StrictlyEnforceRange
    highlightFollowsCurrentItem: true
    flickDeceleration: units.gu(3)
    maximumFlickVelocity: units.gu(500)
    highlightMoveDuration: units.gu(11)
    boundsBehavior: Flickable.DragOverBounds

    onMovingChanged: {
        // TODO: if you scroll through a number of pages without stopping, this
        // never gets updated, so the highlighting stops working.
        if (moving)
            currentIndexForHighlight = currentIndex;
    }

    // Keyboard focus while visible
    onVisibleChanged: {
        if (visible)
            forceActiveFocus();
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Left) {
            decrementCurrentIndex();
            event.accepted = true;
        } else if (event.key === Qt.Key_Right) {
            incrementCurrentIndex();
            event.accepted = true;
        }
    }

    /* FIXME: We need to handle the onWheel event to be able to navigate
       between photos/videos on MediaListView using two fingers swipe on
       desktop touchpad. There is an issue on Qt 5.2 where wheelEvents
       are not able to flick between items of a ListView with snapMode
       set as SnapOneItem. */
    MouseAreaWithMultipoint {
        desktop: APP.desktopMode
        anchors.fill: parent
        enabled: APP.desktopMode && (currentItem.state === "unzoomed" || currentItem.state === "stopped")
        propagateComposedEvents: true

        onWheel: {
            if (wheel.angleDelta.x < 0) {
                decrementCurrentIndex();
                wheel.accepted = true;
            } else if (wheel.angleDelta.x > 0) {
                incrementCurrentIndex();
                wheel.accepted = true;
            }
        }

        onPressed: {
            mouse.accepted = false;
        }
    }
}

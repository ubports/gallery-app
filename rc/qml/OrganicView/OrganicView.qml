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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import "../Utility"
import "../../js/Gallery.js" as Gallery

// A ListView meant to hold OrganicMediaLists in some form or another.
Page {
    id: organicView

    /*!
    */
    signal mediaSourcePressed(var mediaSource, var thumbnailRect)
    signal positionEventsAtBeginning()

    /*!
    */
    property alias model: organicList.model
    /*!
    */
    property alias delegate: organicList.delegate
    /*!
    */
    property SelectionState selection

    // readonly
    // Some duplication from OrganicMediaList, to make certain things easier.
    property int organicMediaListMediaPerPattern: 6 // OrganicMediaList.mediaPerPattern
    /*!
    */
    property real organicMediaListPatternWidth: units.gu(49) // OrganicMediaList.patternWidth
    /*!
    */
    property real organicMediaListMargin: units.gu(1)

    // This property exists only for the benefit of AP tests that
    // can't access C++ objects
    property int _eventCount: model.count

    function positionViewAtBeginning() {
        organicList.positionViewAtBeginning();
        //FIXME Initial content is hide bellow Page Header after a positionViewAtBeginning()
        organicList.contentY = units.gu(7) * -1;
        positionEventsAtBeginning()
    }

    ListView {
        id: organicList

        anchors.fill: parent
        clip: true
        maximumFlickVelocity: units.gu(350)
        flickDeceleration: maximumFlickVelocity * 0.8
        cacheBuffer: 0
        onFlickStarted: cacheBuffer = height * 3;

        // The OrganicMediaList only has a half margin at the top and bottom, since
        // when repeated that means a full margin between rows.  This pads it out
        // so we also get a full row on top and bottom of the whole bunch.
        header: Item {
            width: parent.width
            height: organicMediaListMargin
        }
        footer: Item {
            width: parent.width
            height: organicMediaListMargin
        }

        displaced: Transition {
            NumberAnimation {
                properties: "x,y"
                duration: Gallery.FAST_DURATION
                easing.type: Easing.InQuint
            }
        }
        // TODO: specify add and remove transitions here too.  When I tried
        // initially, QML ignored it.
    }
}

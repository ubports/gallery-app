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

// An item that contains an inside area the maximum size of a particular aspect
// ratio (similar to the Image.PreserveAspectFit fillMode of QtQuick's Image
// element).
Item {
    id: aspectArea

    /*!
    */
    property real aspectWidth
    /*!
    */
    property real aspectHeight

    // The item to place inside (and reparent to) the aspectItem.
    property Item content

    // readonly
    // The item that has the correct aspect ratio inside the AspectArea.
    property Item aspectItem: aspectItem

    // internal
    /*!
    */
    property real aspectRatio: aspectWidth / aspectHeight
    /*!
    */
    property bool isWidthConstrained: (height * aspectRatio > width)

    Component.onCompleted: {
        if (content)
            content.parent = aspectItem;
    }

    Item {
        id: aspectItem

        anchors.centerIn: parent
        width: (isWidthConstrained ? parent.width : parent.height * aspectRatio)
        height: (isWidthConstrained ? parent.width / aspectRatio : parent.height)
    }
}

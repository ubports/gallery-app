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
import "../../js/GalleryUtility.js" as GalleryUtility

// A child delegate for a Checkerboard.  Tightly coupled to Checkerboard due to
// some required interactions (e.g. selection).  This is a separate component
// so you can define your own.
Item {
    id: checkerboardDelegate
    // This name is checked for in Checkerboard.getDelegateInstanceAt().  Please
    // don't modify it.
    objectName: "checkerboardDelegate"

    /*!
    */
    signal swipeStarted(bool leftToRight, int start)
    /*!
    */
    signal swiping(bool leftToRight, int start, int distance)
    /*!
    */
    signal swiped(bool leftToRight)
    /*!
    */
    signal longPressed(variant object)

    /*!
    */
    property variant checkerboard // The owning Checkerboard.
    // This delegate encompasses the content + the gutter.  content is the item
    // (sized checkerboard.itemWidth/Height) inside the gutter.  It will be
    // reparented internally so it's positioned correctly.
    property Item content
    /*!
    */
    property bool contentIsSwipable: false
    // Whether to apply the default selection highlighting internally.
    property bool useInternalSelectionHighlight: true
    /*!
    */
    property bool isEnabled: true

    // readonly
    /*!
    */
    property variant modelData: model
    // Also needed for Checkerboard.getDelegateInstanceAt().
    property int index: modelData.index
    /*!
    */
    property bool isSelected: modelData.isSelected

    width: checkerboard.delegateWidth
    height: checkerboard.delegateHeight

    Component.onCompleted: {
        if (content)
            content.parent = contentArea;
    }

    Item {
        id: contentArea

        width: checkerboard.itemWidth
        height: checkerboard.itemHeight
        anchors.centerIn: parent

        // Long press/right click.
        function alternativePressed() {
            checkerboard.selection.toggleSelection(modelData.object);
            longPressed(modelData.object);
        }

        // Normal press/click.
        function pressed() {
            var rect = GalleryUtility.getRectRelativeTo(contentArea, checkerboard);
            checkerboard.activated(modelData.object, modelData, rect, checkerboardDelegate);
        }

        MouseArea {
            id: mouseArea

            anchors.fill: parent

            visible: !contentIsSwipable && checkerboardDelegate.isEnabled

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressAndHold: contentArea.alternativePressed()
            onClicked: {
                if (mouse.button == Qt.RightButton || checkerboard.selection.inSelectionMode)
                    contentArea.alternativePressed();
                else
                    contentArea.pressed();
            }
        }

        SwipeArea {
            id: swipeArea

            anchors.fill: parent

            visible: contentIsSwipable && checkerboardDelegate.isEnabled

            onLongPressed: contentArea.alternativePressed()
            onTapped: {
                if (rightButton || checkerboard.selection.inSelectionMode)
                    contentArea.alternativePressed();
                else
                    contentArea.pressed();
            }
            onStartSwipe: checkerboardDelegate.swipeStarted(leftToRight, start)
            onSwiping: checkerboardDelegate.swiping(leftToRight, start, distance)
            onSwiped: checkerboardDelegate.swiped(leftToRight)
        }
    }
}

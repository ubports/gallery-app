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

/*!
*/
AlbumPageLayout {
    id: albumPageLayoutLeftDoubleLandscape

    mediaFrames: [ top, bottom ]

    FramePortrait {
        id: top

        anchors.top: parent.top
        anchors.bottom: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.topMargin: albumPageLayoutLeftDoubleLandscape.topMargin
        anchors.bottomMargin: albumPageLayoutLeftDoubleLandscape.insideMargin / 2
        anchors.leftMargin: albumPageLayoutLeftDoubleLandscape.gutterMargin
        anchors.rightMargin: albumPageLayoutLeftDoubleLandscape.outerMargin

        mediaSource: (albumPageLayoutLeftDoubleLandscape.mediaSourceList
                      ? albumPageLayoutLeftDoubleLandscape.mediaSourceList[0]
                      : null)
        load: albumPageLayoutLeftDoubleLandscape.load
        isPreview: albumPageLayoutLeftDoubleLandscape.isPreview
    }

    FramePortrait {
        id: bottom

        anchors.top: parent.verticalCenter
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.topMargin: albumPageLayoutLeftDoubleLandscape.insideMargin / 2
        anchors.bottomMargin: albumPageLayoutLeftDoubleLandscape.bottomMargin
        anchors.leftMargin: albumPageLayoutLeftDoubleLandscape.gutterMargin
        anchors.rightMargin: albumPageLayoutLeftDoubleLandscape.outerMargin

        mediaSource: (albumPageLayoutLeftDoubleLandscape.mediaSourceList
                      ? albumPageLayoutLeftDoubleLandscape.mediaSourceList[1]
                      : null)
        load: albumPageLayoutLeftDoubleLandscape.load
        isPreview: albumPageLayoutLeftDoubleLandscape.isPreview
    }
}

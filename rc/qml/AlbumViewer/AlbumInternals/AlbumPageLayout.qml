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

// AlbumPageLayouts are all loaded from AlbumPageContents, and should inherit
// its properties.
Item {
    id: albumPageLayout

    /*!
    */
    property variant mediaSourceList: (parent ? parent.mediaSourceList : null)

    /*!
    */
    property real topMargin: (parent ? parent.topMargin : 0)
    /*!
    */
    property real bottomMargin: (parent ? parent.bottomMargin : 0)
    /*!
    */
    property real gutterMargin: (parent ? parent.gutterMargin : 0)
    /*!
    */
    property real outerMargin: (parent ? parent.outerMargin : 0)
    /*!
    */
    property real insideMargin: (parent ? parent.insideMargin : 0)

    /*!
    */
    property bool load: (parent ? parent.load : false)
    /*!
    */
    property bool isPreview: (parent ? parent.isPreview : false)

    /*!
    */
    property variant mediaFrames: []

    width: (parent ? parent.width : 0)
    height: (parent ? parent.height : 0)
}

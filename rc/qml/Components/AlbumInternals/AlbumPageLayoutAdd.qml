/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0

// This page layout is used only for empty albums.
AlbumPageLayout {
    id: albumPageLayoutAdd

    mediaFrames: [ addButton ]

    Image {
        id: addButton
        objectName: "addButton" // Don't change this, we check for this name elsewhere

        anchors.centerIn: parent
        source: "img/album-add.png"

        width: parent.width / 4
        height: width
    }
}

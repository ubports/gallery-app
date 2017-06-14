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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.4
import Gallery 1.0

/*!
*/
ListModel {
    id: albumCoverList

    // Returns an element given the nickname.
    function elementForCoverName(name) {
        // TODO: optimize.
        for (var i = 0; i < albumCoverList.count; i++) {
            if (albumCoverList.get(i).coverName === name)
                return albumCoverList.get(i);
        }

        return albumCoverList.get(0); // default
    }

    /*!
    */
    function getDefault() {
        return albumCoverList.get(0);
    }

    // A note about the image paths here: the imageFull and imagePreview images
    // are used in qml/Components/AlbumCover.qml (and the actual cover images are
    // only needed there, so they're in qml/Components/img).  So, our path in
    // those cases is relative to the Components directory.  However, the
    // iconFilename is actually used by qml/Capetown/Widgets/SomethingOrOther
    // (and thus its images live in the shared img directory).  So, our path in
    // that case is relative to the Capetown Widgets directory.
    // TODO: fix this so images don't have such strict requirements; maybe make
    // a js utility function to give us the path to any image from wherever it's
    // being used.

    // First item in the list must be the default.
    ListElement {
        title: ""
        coverName: "default"
        imageFull: "img/album-cover-default-large.png"
        imagePreview: "img/album-cover-default.png"
        iconFilename: "../../img/album-cover-default-icon.png"
        addFilename: "img/album-add-default.png"
        hasBottomBorder: true
    }

    ListElement {
        title: ""
        coverName: "blue"
        imageFull: "img/album-cover-blue-large.png"
        imagePreview: "img/album-cover-blue.png"
        iconFilename: "../../img/album-cover-blue-icon.png"
        addFilename: "img/album-add-blue.png"
        hasBottomBorder: true
    }

    ListElement {
        title: ""
        coverName: "green"
        imageFull: "img/album-cover-green-large.png"
        imagePreview: "img/album-cover-green.png"
        iconFilename: "../../img/album-cover-green-icon.png"
        addFilename: "img/album-add-green.png"
        hasBottomBorder: true
    }

    ListElement {
        title: ""
        coverName: "pattern"
        imageFull: "img/album-cover-pattern-large.png"
        imagePreview: "img/album-cover-pattern.png"
        iconFilename: "../../img/album-cover-pattern-icon.png"
        addFilename: "img/album-add-pattern.png"
        hasBottomBorder: true
    }

    ListElement {
        title: ""
        coverName: "red"
        imageFull: "img/album-cover-red-large.png"
        imagePreview: "img/album-cover-red.png"
        iconFilename: "../../img/album-cover-red-icon.png"
        addFilename: "img/album-add-red.png"
        hasBottomBorder: true
    }

    Component.onCompleted: {
        // Script expressions are not allowed for the property values of a list element
        // The the translated titles are set here
        for (var i = 0; i < albumCoverList.count; i++) {
            var elem = albumCoverList. get(i);
            switch (i) {
                case 0: elem.title = i18n.tr("Default"); break;
                case 1: elem.title = i18n.tr("Blue"); break;
                case 2: elem.title = i18n.tr("Green"); break;
                case 3: elem.title = i18n.tr("Pattern"); break;
                case 4: elem.title = i18n.tr("Red"); break;
                default: elem.title = "";
            }
            albumCoverList.set(i, elem);
        }
    }
}

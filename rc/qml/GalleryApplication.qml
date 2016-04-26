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
 * Charles Lindsay <chaz@yorba.org
 */

import QtQuick 2.4
import QtQuick.Window 2.2
import "../js/Gallery.js" as Gallery
import Ubuntu.Components 1.3 // Necessary to make filename@GU.ext images load

Item {
    id: application

    // readonly
    property bool isPortrait: (height > width)

    property bool automaticOrientation: true

    // Store current requested transfer
    property var transfer: null

    /// Indicates if the backend is loaded and the UI can be shown
    property bool allLoaded: false

    // Big list of form factor-specific values.  'default' is used if the key
    // can't be found under any other form_factor (otherwise, form_factors should
    // match the FORM_FACTOR values passed from main.cpp).  Just add Portrait to
    // the end of a key to make it portrait-mode specific.
    property var deviceSpecifics: {
        'default': {
            // Size of thumbnails in album overview.
            albumThumbnailWidth: units.gu(17),
            albumThumbnailHeight: units.gu(20),

            // Whitespace around photos in album overview.
            albumGridTopMargin: units.gu(1), // Plus half of albumGridGutterHeight.
            albumGridLeftMargin: units.gu(1), // Plus half of albumGridGutterWidth.
            albumGridRightMargin: units.gu(1), // Plus half of albumGridGutterWidth.
            albumGridGutterWidth: units.gu(2),
            albumGridGutterHeight: units.gu(2),

            // Whitespace around photos on album pages in the album viewer.
            albumPageTopMargin: units.gu(2),
            albumPageBottomMargin: units.gu(2),
            albumPageGutterMargin: units.gu(1), // Between spine and photo.
            albumPageGutterMarginPortrait: units.gu(2),
            albumPageOuterMargin: units.gu(2), // Between opposite edge and photo.
            albumPageInsideMargin: units.gu(2), // Between photos on one page.

            // Whitespace around photos on album pages in preview thumbnails.
            albumPreviewTopMargin: units.gu(1),
            albumPreviewBottomMargin: units.gu(1),
            albumPreviewGutterMargin: units.gu(1), // Between spine and photo.
            albumPreviewOuterMargin: units.gu(1), // Between opposite edge and photo.
            albumPreviewInsideMargin: units.gu(1), // Between photos on one page.
        },

        'phone': {
        },

        'tablet': {
            // Size of thumbnails in album overview.
            albumThumbnailWidth: units.gu(25),
            albumThumbnailHeight: units.gu(30),

            // Whitespace around photos in album overview.
            albumGridTopMargin: units.gu(1), // Plus half of albumGridGutterHeight.
            albumGridLeftMargin: units.gu(1), // Plus half of albumGridGutterWidth.
            albumGridRightMargin: units.gu(1), // Plus half of albumGridGutterWidth.
            albumGridGutterWidth: units.gu(3),
            albumGridGutterHeight: units.gu(3),

            // Whitespace around photos on album pages in the album viewer.
            albumPageTopMargin: units.gu(2),
            albumPageBottomMargin: units.gu(2),
            albumPageGutterMargin: units.gu(1), // Between spine and photo.
            albumPageGutterMarginPortrait: units.gu(2),
            albumPageOuterMargin: units.gu(2), // Between opposite edge and photo.
            albumPageInsideMargin: units.gu(2), // Between photos on one page.

            // Whitespace around photos on album pages in preview thumbnails.
            albumPreviewTopMargin: units.gu(1.5),
            albumPreviewBottomMargin: units.gu(1.5),
            albumPreviewGutterMargin: units.gu(2.5), // Between spine and photo.
            albumPreviewOuterMargin: units.gu(2.5), // Between opposite edge and photo.
            albumPreviewInsideMargin: units.gu(1.5), // Between photos on one page.
        },
    };

    function getDeviceSpecific(key, form_factor, is_portrait) {
        if (is_portrait === undefined)
            is_portrait = isPortrait; // From GalleryApplication.

        if (!form_factor)
            form_factor = FORM_FACTOR; // From C++.
        if (form_factor === 'sidebar')
            form_factor = 'phone'; // Equivalent; saves typing above.
        if (deviceSpecifics[form_factor] === undefined) {
            if (Screen.primaryOrientation === Qt.LandscapeOrientation)
                form_factor = 'tablet';
            else
                form_factor = 'default';
        }

        var portrait_key = key + 'Portrait';
        if (is_portrait && deviceSpecifics[form_factor][portrait_key] !== undefined)
            return deviceSpecifics[form_factor][portrait_key];
        if (is_portrait && deviceSpecifics['default'][portrait_key] !== undefined)
            return deviceSpecifics['default'][portrait_key];

        if (deviceSpecifics[form_factor][key] !== undefined)
            return deviceSpecifics[form_factor][key];
        if (deviceSpecifics['default'][key] !== undefined)
            return deviceSpecifics['default'][key];

        console.debug("Invalid key '" + key + "' passed to getDeviceSpecific()");
        return undefined;
    }

    // units.gu(), but for font point sizes.
    function pointUnits(amt) {
        // 1.6 is a hacky fudge-factor for now.
        return Math.round(1.6 * amt);
    }

    function onLoaded() {
        allLoaded = true;
    }

    function swapDimensions() {
        var temp = width;
        width = height;
        height = temp;
    }

    function setFullScreenAppMode(fullScreen) {
        APP.fullScreenAppMode = fullScreen;
    }

    function setFullScreenUserMode(fullScreen) {
        APP.fullScreenUserMode = fullScreen;
    }

    width: units.gu(DEVICE_WIDTH)
    height: units.gu(DEVICE_HEIGHT)

    focus: true
    Keys.onPressed: {
        // Key presses only makes sense on Desktop
        if (!Gallery.isDesktop()) {
            return;
        }

        switch(event.key) {
        case Qt.Key_F11:
            setFullScreenUserMode(!APP.fullScreenUserMode);
            break;

        case Qt.Key_Escape:
            setFullScreenUserMode(false);
            break;

        case Qt.Key_L:
            if (isPortrait) {
                swapDimensions();
            }
            break;

        case Qt.Key_P:
            if (!isPortrait) {
                swapDimensions();
            }
            break;

        default:
            break;
        }
    }

    Loader {
        id: mainScreenLoader
        anchors.fill: parent
        visible: status === Loader.Ready
        source: APP.pickModeEnabled ? Qt.resolvedUrl("PickerScreen.qml") : Qt.resolvedUrl("MainScreen.qml")
    }

    Binding { 
        target: i18n
        property: "domain"
        value: "gallery-app" 
    }
}

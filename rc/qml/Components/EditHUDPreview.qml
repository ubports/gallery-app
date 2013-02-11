/*
 * Copyright (C) 2013 Canonical Ltd
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
 */

import QtQuick 2.0
import Gallery 1.0
import "../Utility"

Item {
    id: root

    /// The current photo to be edited
    property variant photo: null

    EditPreview {
        id: editPreview
        anchors.fill: parent
        source: photo ? photo.galleryPreviewPath : ""

        visible: editHUD.actionActive

        exposure: editHUD.exposureValue

        brightness: editHUD.brightness
        contrast: editHUD.contrast
        saturation: editHUD.saturation
        hue: editHUD.hue
    }

    EditingHUD {
        id: editHUD
        photo: root.photo
        onExposureActivated: root.useExposure()
        onColorBalanceActivated: root.useColorBalance()
    }
}

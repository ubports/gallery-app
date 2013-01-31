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
import Ubuntu.HUD 0.1 as HUD

Item {
    id: root

    /*!
    */
    property variant photo: null

    HUD.HUD {
        id: hud
        applicationIdentifier: "gallery"

        HUD.Context {
            id: ctx

            HUD.Action {
                id: cropAction
                label: "Crop"
                description: "Crop the image"
                onTriggered: {
                    console.log(description)
                    photo.crop()
                }
            }

            HUD.Action {
                id: autoAdjustAction
                label: "Auto Adjust"
                description: "Adjust the image automatically"
                onTriggered: {
                    console.log(description)
                    photo.autoEnhance()
                }
            }

            HUD.Action {
                id: rotateAction
                label: "Rotate"
                description: "Rotate the image clockwise"
                onTriggered: {
                    console.log(description)
                    photo.rotateRight()
                }
            }
        }
    }
}

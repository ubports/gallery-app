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

            HUD.Action {
                id: exposureAction
                label: "Exposure"
                description: "lorem ipsum"
                hasLivePreview: true
                commitLabel: "Confirm" // string to show in the confirm button

                HUD.SliderParameter {
                    id: compensationParam
                    label: "Compensation"

                    minimumValue: 0.0
                    maximumValue: 100.0
                    value: 50.0
                    live: true

                    onValueChanged: {
                        // update the preview
                        console.debug("Compensation: " + value)
                    }
                }

                onStarted: {
                    // this gets triggered when the user selects the action from the HUD
                    // and the parameter view is opened / preview begins
                    // this is probably a good place to reset the parameters to initial values
                    console.debug("exposure started")
                }

                onResetted: {
                    // this gets triggered when user presses the reset button
                    // NOT ON THE SCRIPT
                    console.debug("exposure resetted")
                }

                onCancelled: {
                    // this gets triggered when user leaves the parameter view / preview
                    // without confirming the action
                    console.debug("exposure cancelled")
                }

                onTriggered: {
                    // this get triggered when user presses the "confirm" button
                    // and the action should be carried out permanently
                    console.debug("exposure triggered")
                }
            }


            HUD.Action {
                id: colorBalanceAction
                label: "Color Balance"
                description: "lorem ipsum"
                hasLivePreview: true
                commitLabel: "Confirm"

                HUD.SliderParameter {
                    id: brightnessParam
                    label: "Brightness"

                    minimumValue: 0.0
                    maximumValue: 100.0
                    value: 50.0
                    live: true

                    onValueChanged: {
                        // update the preview
                        console.debug("Brightness: " + value)
                    }
                }

                HUD.SliderParameter {
                    id: contrastParam
                    label: "Contrast"

                    minimumValue: 0.0
                    maximumValue: 100.0
                    value: 50.0
                    live: true

                    onValueChanged: {
                        // update the preview
                        console.debug("Contrast: " + value)
                    }
                }

                HUD.SliderParameter {
                    id: saturationParam
                    label: "Saturation"

                    minimumValue: 0.0
                    maximumValue: 100.0
                    value: 50.0
                    live: true

                    onValueChanged: {
                        // update the preview
                        console.debug("Saturation: " + value)
                    }
                }

                HUD.SliderParameter {
                    id: hueParam
                    label: "Hue"

                    minimumValue: 0.0
                    maximumValue: 100.0
                    value: 50.0
                    live: true

                    onValueChanged: {
                        // update the preview
                        console.debug("Hue: " + value)
                    }
                }

                onStarted: {
                    // this gets triggered when the user selects the action from the HUD
                    // and the parameter view is opened / preview begins
                    // this is probably a good place to reset the parameters to initial values
                    console.debug("color balance started")
                }

                onResetted: {
                    // this gets triggered when user presses the reset button
                    // NOT ON THE SCRIPT
                    console.debug("color balance resetted")
                }

                onCancelled: {
                    // this gets triggered when user leaves the parameter view / preview
                    // without confirming the action
                    console.debug("color balance cancelled")
                }

                onTriggered: {
                    // this get triggered when user presses the "confirm" button
                    // and the action should be carried out permanently
                    console.debug("color balance triggreed")
                }
            }
        }
    }
}

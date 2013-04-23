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
import Ubuntu.HUD 1.0 as HUD

/*!
  EditingHUD is an itme to connect to the HUD services.
  It exports all possible actions and triggers the edit functionality inside of gallery.
  */
Item {
    id: root

    /// The current photo to be edited
    property variant photo: null
    /// Is true when a HUD action with parameters is active
    property bool actionActive: false

    /// Exposure compensation from -1.0 (dark) to +1.0 (bright)
    property double exposureValue: 0.0

    /// Brightnes value of the color balance
    property double brightness: 0.0
    /// Contrast value of the color balance
    property double contrast: 0.0
    /// Saturation value of the color balance
    property double saturation: 0.0
    /// Hue value of the color balance
    property double hue: 0.0

    /// Emitted when the exposure action is started
    signal exposureActivated()
    /// Emitted when the color balance action is started
    signal colorBalanceActivated()

    Component.onCompleted: {
        hudCtx.addAction(deleteAction);
        hudCtx.addAction(shareAction);
        hudCtx.addAction(addAction);
        hudCtx.addAction(undoAction);
        hudCtx.addAction(redoAction);
        hudCtx.addAction(autoEnhanceAction);
        hudCtx.addAction(rotateAction);
        hudCtx.addAction(cropAction);
        hudCtx.addAction(revertAction);
        hudCtx.addAction(exposureAction);
        hudCtx.addAction(colorBalanceAction);
    }

    HUD.Action {
        id: deleteAction
        label: i18n.tr("Delete")
        keywords: "Trash;Erase"
    }
    HUD.Action {
        id: shareAction
        label: i18n.tr("Share")
        keywords: "Post;Upload;Attach"
    }
    HUD.Action {
        id: addAction
        label: i18n.tr("Add")
        keywords: "Add Photo to Album"
    }
    HUD.Action {
        id: undoAction
        label: i18n.tr("Undo")
        keywords: "Cancel Action;Backstep"
    }
    HUD.Action {
        id: redoAction
        label: i18n.tr("Redo")
        keywords: "Reapply;Make Again"
    }
    HUD.Action {
        id: autoEnhanceAction
        label: i18n.tr("Auto Enhance")
        description: "Adjust the image automatically"
        keywords: "Automatically Adjust Photo"
        onTriggered: photo.autoEnhance()
    }
    HUD.Action {
        id: rotateAction
        label: i18n.tr("Rotate")
        keywords: "Turn Clockwise"
        description: "Rotate the image clockwise"
        onTriggered: photo.rotateRight()
    }
    HUD.Action {
        id: cropAction
        label: i18n.tr("Crop")
        keywords: "Trim;Cut"
        description: "Crop the image"
        onTriggered: cropper.show(photo)
    }
    HUD.Action {
        id: revertAction
        label: i18n.tr("Revert to Original")
        keywords: "Discard Changes"
        description: "Discard all changes"
    }
    HUD.Action {
        id: exposureAction
        label: i18n.tr("Exposure")
        description: "Adjust the exposure"
        keywords: "Underexposed;Overexposed"
        hasLivePreview: true
        commitLabel: "Confirm" // string to show in the confirm button

        HUD.SliderParameter {
            id: compensationParam
            label: i18n.tr("Compensation")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 50.0
            live: true

            onValueChanged: root.exposureValue = (value / 50.0) - 1.0
        }

        onStarted: {
            root.exposureActivated()
            resetValues()
            root.actionActive = true
        }

        onResetted: compensationParam.value = 50

        onCancelled: {
            compensationParam.value = 50
            root.actionActive = false
        }

        function resetValues() {
            root.exposureValue = 0.0
        }

        onTriggered: {
            photo.exposureCompensation(root.exposureValue)
            root.actionActive = false
        }
    }
    HUD.Action {
        id: colorBalanceAction
        label: i18n.tr("Color Balance")
        description: "Adjust color balance"
        keywords: "Saturation;Hue"
        hasLivePreview: true
        commitLabel: "Confirm"

        HUD.SliderParameter {
            id: brightnessParam
            label: i18n.tr("Brightness")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 20.0
            live: true

            onValueChanged: root.brightness = value / 20.0
        }

        HUD.SliderParameter {
            id: contrastParam
            label: i18n.tr("Contrast")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 20.0
            live: true

            onValueChanged: root.contrast = value / 20.0
        }

        HUD.SliderParameter {
            id: saturationParam
            label: i18n.tr("Saturation")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 20.0
            live: true

            onValueChanged: root.saturation = value / 20.0
        }

        HUD.SliderParameter {
            id: hueParam
            label: i18n.tr("Hue")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 0.0
            live: true

            onValueChanged: root.hue = value * 3.6
        }

        onStarted: {
            root.colorBalanceActivated()
            resetValues()
            root.actionActive = true
        }

        onResetted: {
            resetValues()
        }

        function resetValues() {
            brightnessParam.value = 20
            root.brightness = 1.0
            contrastParam.value = 20
            root.contrast = 1.0
            saturationParam.value = 20
            saturation = 1.0
            hueParam.value = 0
            root.hue = 0.0
        }

        onCancelled: root.actionActive = false

        onTriggered: {
            photo.colorBalance(root.brightness, root.contrast, root.saturation, root.hue)
            root.actionActive = false
        }
    }
}

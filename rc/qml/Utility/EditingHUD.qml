/*
 * Copyright (C) 2013-2015 Canonical Ltd
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

import QtQuick 2.4
import Gallery 1.0
import Ubuntu.Components 1.3

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

    Action {
        id: deleteAction
        text: i18n.tr("Delete")
        keywords: i18n.tr("Trash;Erase")
    }
    Action {
        id: shareAction
        text: i18n.tr("Share")
        keywords: i18n.tr("Post;Upload;Attach")
    }
    Action {
        id: addAction
        text: i18n.tr("Add")
        keywords: i18n.tr("Add Photo to Album")
    }
    Action {
        id: undoAction
        text: i18n.tr("Undo")
        keywords: i18n.tr("Cancel Action;Backstep")
    }
    Action {
        id: redoAction
        text: i18n.tr("Redo")
        keywords: i18n.tr("Reapply;Make Again")
    }
    Action {
        id: autoEnhanceAction
        text: i18n.tr("Auto Enhance")
        description: i18n.tr("Adjust the image automatically")
        keywords: i18n.tr("Automatically Adjust Photo")
        onTriggered: photo.autoEnhance()
    }
    Action {
        id: rotateAction
        text: i18n.tr("Rotate")
        keywords: i18n.tr("Turn Clockwise")
        description: i18n.tr("Rotate the image clockwise")
        onTriggered: photo.rotateRight()
    }
    Action {
        id: cropAction
        text: i18n.tr("Crop")
        keywords: i18n.tr("Trim;Cut")
        description: i18n.tr("Crop the image")
        onTriggered: cropper.show(photo)
    }
    Action {
        id: revertAction
        text: i18n.tr("Revert to Original")
        keywords: i18n.tr("Discard Changes")
        description: i18n.tr("Discard all changes")
    }
    PreviewAction {
        id: exposureAction
        text: i18n.tr("Exposure")
        description: i18n.tr("Adjust the exposure")
        keywords: i18n.tr("Underexposed;Overexposed")
        commitLabel: i18n.tr("Confirm") // string to show in the confirm button

        PreviewRangeParameter {
            id: compensationParam
            text: i18n.tr("Compensation")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 50.0

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

    PreviewAction {
        id: colorBalanceAction
        text: i18n.tr("Color Balance")
        description: i18n.tr("Adjust color balance")
        keywords: i18n.tr("Saturation;Hue")
        commitLabel: i18n.tr("Confirm")

        PreviewRangeParameter {
            id: brightnessParam
            text: i18n.tr("Brightness")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 20.0

            onValueChanged: root.brightness = value / 20.0
        }

        PreviewRangeParameter {
            id: contrastParam
            text: i18n.tr("Contrast")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 20.0

            onValueChanged: root.contrast = value / 20.0
        }

        PreviewRangeParameter {
            id: saturationParam
            text: i18n.tr("Saturation")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 20.0

            onValueChanged: root.saturation = value / 20.0
        }

        PreviewRangeParameter {
            id: hueParam
            text: i18n.tr("Hue")

            minimumValue: 0.0
            maximumValue: 100.0
            value: 0.0

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

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
import Ubuntu.Components 1.1
import "../../js/Gallery.js" as Gallery
import "../OrganicView"
import "../Utility"

/*!
  MediaSelector provides a view, with all photos. And these can be selected.
*/
OrganicView {
    id: mediaSelector

    /// Is true, while the animation tto show/hide the media selector is running
    property alias animationRunning: blendAnimation.running

    /// Emitted when the user clicked the add button
    signal addClicked()
    /// Emitted when fully shown
    signal shown()
    /// Emitted when fully hidden
    signal hidden()

    onActiveChanged: {
        if (active)
            header.show();
    }

    /// Shows the item in an animated way
    function show() {
        visible = true;
        opacity = 0;
        blendAnimation.to = 1;
        blendAnimation.start();
        header.show();
    }
    /// Hides the item in an animated way
    function hide() {
        blendAnimation.to = 0;
        blendAnimation.start();
    }

    title: i18n.tr("Add to Album")
    visible: false

    selection: SelectionState {
        id: select
        inSelectionMode: true
        allowSelectionModeChange: false
    }

    model: EventCollectionModel {
    }

    delegate: OrganicMediaList {
        width: mediaSelector.width
        event: model.event
        selection: mediaSelector.selection
    }

    head.actions: [
        Action {
            objectName: "addButton"
            text: i18n.tr("Add to Album")
            iconName: "add"
            enabled: mediaSelector.selection.selectedCount
            onTriggered: {
                mediaSelector.addClicked();
                mediaSelector.hide();
            }
        }
    ]

    head.backAction: Action {
        objectName: "cancelButton"
        text: i18n.tr("Cancel")
        iconName: "back"
        onTriggered: {
            mediaSelector.hide();
        }
    }

    PropertyAnimation {
        id: blendAnimation
        target: mediaSelector
        property: "opacity"
        duration: Gallery.FAST_DURATION
        easing.type: Easing.InQuint
        onStopped: {
            if (mediaSelector.opacity === 1) {
                mediaSelector.shown();
            } else {
                selection.unselectAll();
                visible = false;
                mediaSelector.hidden();
            }
        }
    }
}

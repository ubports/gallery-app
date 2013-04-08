/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import "../Capetown"
import "../js/Gallery.js" as Gallery
import "Components"
import "Utility"
import "Widgets"

/*!
  MediaSelector provides a view, with all photos. And these can be selected.
*/
OrganicView {
    id: mediaSelector

    /// Emiotted when the user clicked the add button
    signal addClicked()
    /// Emitted when fully shown
    signal shown()
    /// Emitted when fully hidden
    signal hidden()

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

    tools: toolActions
    title: i18n.tr("Add to Album")
    flickable: null

    selection: SelectionState {
        id: select
        inSelectionMode: true
        allowSelectionModeChange: false
    }

    model: EventCollectionModel {
    }

    organicMediaListMargin: header.height + units.gu(9)

    delegate: OrganicMediaList {
        width: mediaSelector.width

        animationDuration: mediaSelector.animationDuration
        animationEasingType: mediaSelector.animationEasingType

        event: model.event
        selection: mediaSelector.selection
    }

    property ToolbarActions toolActions: ToolbarActions {
        Action {
            text: i18n.tr("Add to Album")
            iconSource: "../img/add.png"
            onTriggered: {
                mediaSelector.addClicked();
                mediaSelector.hide();
            }
        }

        back: Action {
            itemHint: Button {
                text: i18n.tr("Cancel")
                width: units.gu(10)
            }
            onTriggered: {
                mediaSelector.hide();
            }
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
                toolbar.active = true;
                mediaSelector.shown();
                console.log("HH: "+header.height + " GU: " + units.gu(1))
            } else {
                selection.unselectAll();
                mediaSelector.hidden();
            }
        }
    }
}

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
import Ubuntu.Components 0.1
import Gallery 1.0
import "Components"
import "OrganicView"
import "Utility"
import "../js/Gallery.js" as Gallery

/*!
The main view for picking content
*/
MainView {
    id: pickerMainView
    objectName: "pickerMainView"

    /// Model of all media
    property MediaCollectionModel mediaLibrary: MediaCollectionModel {
        monitored: true
    }
    /// Holds the selection
    property SelectionState selection: SelectionState {
        inSelectionMode: true
        singleSelect: false
        model: mediaLibrary
    }

    anchors.fill: parent
    applicationName: "gallery-app"
    automaticOrientation: application.automaticOrientation

    Tabs {
        id: tabs
        anchors.fill: parent

        Tab {
            title: i18n.tr("Events")
            page: OrganicView {
                id: eventSelectView
                objectName: "eventSelectView"

                tools: pickTools
                selection: pickerMainView.selection
                model: EventCollectionModel {
                }

                delegate: OrganicMediaList {
                    width: eventSelectView.width
                    event: model.event
                    selection: eventSelectView.selection
                }
            }
        }

        Tab {
            title: i18n.tr("Photos")
            page: Page {
                id: photosOverview

                tools: pickTools

                Image {
                    anchors.fill: parent
                    source: "../img/background-paper.png"
                    fillMode: Image.Tile
                }

                MediaGrid {
                    anchors.fill: parent
                    model: mediaLibrary
                    selection: pickerMainView.selection
                }
            }
        }
    }

    property ToolbarItems pickTools: ToolbarItems {
        Button {
            anchors.verticalCenter: parent.verticalCenter
            text: i18n.tr("Pick")
            objectName: "pickButton"
            color: Gallery.HIGHLIGHT_BUTTON_COLOR
            width: units.gu(16)
            enabled: pickerMainView.selection.selectedCount > 0
            onClicked: {
                if (!enabled)
                    return;

                MANAGER.returnPickedContent(mediaLibrary.selectedMedias);
            }
        }

        back: Button {
            anchors.verticalCenter: parent.verticalCenter
            text: i18n.tr("Cancel")
            objectName: "cancelButton"
            width: units.gu(10)
            onClicked: {
                MANAGER.contentPickingCanceled()
            }
        }
        opened: true
        locked: true
    }
}

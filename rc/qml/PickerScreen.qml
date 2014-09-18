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
import Ubuntu.Components 1.1
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

    useDeprecatedToolbar: false

    Loader {
        id: mediaLibraryLoader
        sourceComponent: allLoaded ? mediaLibraryComponent : ""        

        Component {
            id: mediaLibraryComponent
            MediaCollectionModel {
                monitored: true
                mediaTypeFilter: APP.mediaTypeFilter
            }
        }
    }

    Loader {
        id: selectionLoader
        sourceComponent: allLoaded ? selectionComponent : ""
      
        Component {
            id: selectionComponent
            SelectionState {
                inSelectionMode: true
                singleSelect: PICKER_HUB.singleContentPickMode
                model: mediaLibraryLoader.item
            }
        }
    }

    anchors.fill: parent
    //applicationName: "gallery-app"
    automaticOrientation: application.automaticOrientation

    Tabs {
        id: tabs
        anchors.fill: parent

        StateSaver.properties: "selectedTabIndex"

        Tab {
            title: i18n.tr("Events")
            objectName: "eventsTab"
            page: Loader {
                id: eventSelectViewLoader
                objectName: 'eventSelectViewLoader'
                anchors.fill: parent
                sourceComponent: allLoaded ? eventSelectViewComponent : loadingScreenComponent

                Component {
                    id: eventSelectViewComponent

                    OrganicView {
                        id: eventSelectView
                        objectName: "eventSelectView"

                        head.actions: pickActions

                        selection: allLoaded ? selectionLoader.item : ""
                        model: EventCollectionModel {
                            mediaTypeFilter: APP.mediaTypeFilter
                        }

                        delegate: OrganicMediaList {
                            width: eventSelectView.width
                            event: model.event
                            selection: eventSelectView.selection
                            mediaTypeFilter: APP.mediaTypeFilter
                        }
                    }
                }
            }
        }

        Tab {
            title: i18n.tr("Photos")
            objectName: "photosTab"
            page: Loader {
                id: photosOverviewLoader
                objectName: 'photosOverviewLoader'
                anchors.fill: parent
                sourceComponent: allLoaded ? photosOverviewComponent : loadingScreenComponent

                Component {
                    id: photosOverviewComponent
            
                    Page {
                        id: photosOverview
                        objectName: "photosPage"

                        head.actions: pickActions

                        Image {
                            anchors.fill: parent
                            source: "../img/background-paper.png"
                            fillMode: Image.Tile
                        }

                        MediaGrid {
                            anchors.fill: parent
                            model: allLoaded ? mediaLibraryLoader.item : ""
                            selection: allLoaded ? selectionLoader.item : ""
                        }
                    }
                }
            }
        }
    }

    Component {
        id: loadingScreenComponent
        LoadingScreen {
            id: loadingScreen
            anchors.fill: parent
        }
    }

    property list<Action> pickActions: [
        Action {
            text: i18n.tr("Pick")
            objectName: "pickButton"
            enabled: allLoaded ? selectionLoader.item.selectedCount > 0 : false
            iconName: "ok"
            onTriggered: {
                if (!enabled)
                    return;
                if (allLoaded)
                    APP.returnPickedContent(mediaLibraryLoader.item.selectedMedias);
            }
        },
        Action {
            text: i18n.tr("Cancel")
            objectName: "cancelButton"
            iconName: "close"
            onTriggered: APP.contentPickingCanceled()
        }
    ]
}

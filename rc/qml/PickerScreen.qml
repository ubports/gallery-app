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
import "../js/GalleryUtility.js" as GalleryUtility

/*!
The main view for picking content
*/
MainView {
    id: overview
    objectName: "pickerMainView"

    useDeprecatedToolbar: false

    function setHeaderVisibility(visible)
    {
        header.visible = visible;    
    }

    function toggleHeaderVisibility()
    {
        setHeaderVisibility(!header.visible);    
    }

    function pushPage(page) {
        pageStack.push(page);
    }

    function popPage() {
        pageStack.pop();
    }

    PageStack {
        id: pageStack
        anchors.fill: parent
    }

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
    applicationName: "com.ubuntu.gallery"
    automaticOrientation: application.automaticOrientation

    Component.onCompleted: {
        pageStack.push(tabs);
    }

    Tabs {
        id: tabs
        anchors.fill: parent

        StateSaver.properties: "selectedTabIndex"

        Tab {
            id: eventsTab
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
                            id: organicList
                            width: eventSelectView.width
                            event: model.event
                            selection: eventSelectView.selection
                            mediaTypeFilter: APP.mediaTypeFilter
                            onPressed: {
                                var rect = GalleryUtility.translateRect(thumbnailRect, organicList,
                                eventSelectView);
                                eventSelectView.mediaSourcePressed(mediaSource, rect);
                            }
                        }

                        onMediaSourcePressed: {
                            photoViewerLoader.load();
                            var rect = GalleryUtility.translateRect(thumbnailRect,
                                                                    eventSelectView,
                                                                    photoViewerLoader);
                            photoViewerLoader.item.title = i18n.tr("Select");
                            photoViewerLoader.item.selection = selection;
                            photoViewerLoader.item.animateOpen(mediaSource, rect);
                        }
                    }
                }
            }
        }

        Tab {
            id: photosTab
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

                        signal mediaSourcePressed(var mediaSource, var thumbnailRect)

                        Connections {
                            target: photoViewerLoader.item
                            onSelected: photosGrid.positionViewAtIndex(index, GridView.Contain);
                        }

                        Image {
                            anchors.fill: parent
                            source: "../img/background-paper.png"
                            fillMode: Image.Tile
                        }

                        MediaGrid {
                            id: photosGrid
                            anchors.fill: parent
                            model: allLoaded ? mediaLibraryLoader.item : ""
                            selection: allLoaded ? selectionLoader.item : ""
                        }

                        onMediaSourcePressed: {
                            photoViewerLoader.load();
                            var rect = GalleryUtility.translateRect(thumbnailRect,
                                                                    photosOverview,
                                                                    photoViewerLoader);
                            photoViewerLoader.item.title = i18n.tr("Select");
                            photoViewerLoader.item.selection = photosGrid.selection;
                            photoViewerLoader.item.animateOpen(mediaSource, rect);
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

    Loader {
        id: photoViewerLoader

        anchors.fill: parent
        z: 100

        function load() {
            setSource(Qt.resolvedUrl("MediaViewer/PopupPhotoViewer.qml"), {model: mediaLibraryLoader.item});
        }

        Connections {
            target: photoViewerLoader.item
            onCloseRequested: {
                if (!APP.desktopMode)
                    setFullScreen(false);
                popPage();
                photoViewerLoader.item.fadeClosed();
            }
        }
    }

    property list<Action> pickActions: [
        Action {
            text: i18n.tr("Cancel")
            objectName: "cancelButton"
            iconName: "close"
            onTriggered: APP.contentPickingCanceled()
        },
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
        }
    ]
}

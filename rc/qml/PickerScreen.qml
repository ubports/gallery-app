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
import Ubuntu.Components 1.3
import Ubuntu.Content 1.3
import Gallery 1.0
import "Components"
import "Controller"
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

    property var transfer: application.transfer

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

    function getMediaTypeFilter() {
        if (!overview.transfer) {
            return MediaSource.None
        }

        if (overview.transfer.contentType === ContentType.Pictures) {
            return MediaSource.Photo
        } else if (overview.transfer.contentType === ContentType.Videos) {
            return MediaSource.Video
        }

        return MediaSource.None
    }

    function confirmExport() {
        var item
        var items = []

        var selectedMedias = mediaLibraryLoader.item.selectedMediasQML
        for (var i = 0; i < selectedMedias.length; i++) {
            item = contentItemComponent.createObject(overview.transfer, {"url": selectedMedias[i].path})
            items.push(item)
        }

        overview.transfer.items = items
        overview.transfer.state = ContentTransfer.Charged
        overview.transfer = null

        APP.pickModeEnabled = false
    }

    function cancelExport() {
        if (overview.transfer) {
             overview.transfer.state = ContentTransfer.Aborted
             overview.transfer = null
        }

        APP.pickModeEnabled = false
    }

    SettingsController { id: settings }
    
    PageStack {
        id: pageStack
        anchors.fill: parent
    }

    Loader {
        id: eventCollectionModelLoader
        sourceComponent: allLoaded ? eventCollectionModelComponent : ""        

        Component {
            id: eventCollectionModelComponent
            EventCollectionModel {
                mediaTypeFilter: getMediaTypeFilter()
            }
        }
    }

    Loader {
        id: mediaLibraryLoader
        sourceComponent: allLoaded ? mediaLibraryComponent : ""        

        Component {
            id: mediaLibraryComponent
            MediaCollectionModel {
                monitored: true
                mediaTypeFilter: getMediaTypeFilter()
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
                singleSelect: overview.transfer ? overview.transfer.selectionType === ContentTransfer.Single : false
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
                        model: allLoaded && eventCollectionModelLoader.status == Loader.Ready ? eventCollectionModelLoader.item : ""

                        delegate: OrganicMediaList {
                            id: organicList
                            width: eventSelectView.width
                            event: model.event
                            selection: eventSelectView.selection
                            mediaTypeFilter: getMediaTypeFilter()
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

                        MediaGrid {
                            id: photosGrid
                            property var gridSize: units.gu(settings.gridUnits)
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
                    setFullScreenAppMode(false);
                popPage();
                photoViewerLoader.item.fadeClosed();
            }
        }
    }

    Component {
        id: contentItemComponent
        ContentItem {}
    }

    property list<Action> pickActions: [
        Action {
            text: i18n.tr("Cancel")
            objectName: "cancelButton"
            iconName: "close"
            onTriggered: cancelExport()
        },
        Action {
            text: i18n.tr("Pick")
            objectName: "pickButton"
            enabled: allLoaded ? selectionLoader.item.selectedCount > 0 : false
            iconName: "ok"
            onTriggered: confirmExport()
        }
    ]
}

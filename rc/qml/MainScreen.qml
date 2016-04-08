/*
 * Copyright (C) 2012-2015 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Content 1.3
import Ubuntu.Unity.Action 1.0 as UnityActions
import Ubuntu.Content 1.3
import Gallery 1.0
import "../js/GalleryUtility.js" as GalleryUtility
import "AlbumEditor"
import "AlbumViewer"

MainView {
    id: overview
    objectName: "overview"

    anchors.fill: parent
    applicationName: "com.ubuntu.gallery"
    automaticOrientation: application.automaticOrientation

    property string mediaCurrentlyInView
    StateSaver.properties: "mediaCurrentlyInView"

    property bool applicationLoaded: application.allLoaded

    //fullScreen property is used on autopilot tests
    property bool fullScreen: APP.fullScreenAppMode || APP.fullScreenUserMode
   
    property alias currentPage: pageStack.currentPage

    function openMediaFile(media) {
        if (__isPhotoViewerOpen) {
            popPage();
            photoViewerLoader.item.fadeClosed();
        }

        mediaCurrentlyInView = media;
        for (var i = 0; i < mediaLibraryLoader.item.count; i++) {
            if (mediaLibraryLoader.item.getAt(i).path == mediaCurrentlyInView) {
                photoViewerLoader.load();
                if (tabs.selectedTabIndex === 0) {
                    if (albumsTab.isAlbumOpened) {
                        photoViewerLoader.item.title = i18n.tr("Album");
                    } else {
                        photoViewerLoader.item.title = albumsTab.title;
                    }
                } else if (tabs.selectedTabIndex === 1) {
                    photoViewerLoader.item.title = eventTab.title;
                } else {
                    photoViewerLoader.item.title = photosTab.title;
                }
                photoViewerLoader.item.animateOpen(mediaLibraryLoader.item.getAt(i),
                                                   Qt.rect(0,0,0,0));
                return;
            }
        }
    }

    Component.onCompleted: {
        pageStack.push(tabs);
    }

    onApplicationLoadedChanged: {
        if (applicationLoaded) {
            if (APP.mediaFile !== "")
                openMediaFile(APP.mediaFile);
            else if (mediaCurrentlyInView !== "")
                openMediaFile(mediaCurrentlyInView);
        }
    }

    Connections {
        target: APP
        onEventsViewRequested: {
            if (!allLoaded) {
                return;
            }

            if (__isPhotoViewerOpen) {
                photoViewerLoader.item.closeRequested();
            }

            if (albumsCheckerboardLoader.item) {
                albumsCheckerboardLoader.item.closeAlbum();
            }

            if (tabs.selectedTabIndex == 0) {
                // Move from Albums Tab to Events Tab
                tabs.selectedTabIndex = 1;
            }

            if (tabs.selectedTabIndex == 1 && eventsOverviewLoader.item) {
                eventsOverviewLoader.item.positionViewAtBeginning();
            }

            if (tabs.selectedTabIndex == 2 && eventsOverviewLoader.item) {
                photosOverviewLoader.item.positionViewAtBeginning();
            }
        }
    }

    function pushPage(page, properties) {
        return pageStack.push(page, properties);
    }

    function popPage() {
        pageStack.pop();
    }

    Timer {
        id: setFullScreenTimer
        interval: 10;
        onTriggered: setFullScreenAppMode(!header.visible)
    }

    PageStack {
        id: pageStack
        anchors.fill: parent
    }

    Tabs {
        id: tabs
        anchors.fill: parent

        visible: !(photoViewerLoader.item && photoViewerLoader.item.isPoppedUp)

        selectedTabIndex: 1
        StateSaver.properties: "selectedTabIndex"

        Tab {
            id: albumsTab
            objectName: "albumsTab"
            title: i18n.tr("Albums")
            property bool isAlbumOpened: page.item ? page.item.isAlbumOpened : false
            page: Loader {
                id: albumsCheckerboardLoader
                objectName: "albumsCheckerboardLoader"
                anchors.fill: parent
                source: allLoaded ? Qt.resolvedUrl("AlbumsOverview.qml") : Qt.resolvedUrl("LoadingScreen.qml")
                asynchronous: true
            }
        }

        Tab {
            id: eventTab
            objectName: "eventsTab"
            title: i18n.tr("Events")
            page: Loader {
                id: eventsOverviewLoader
                objectName: 'eventsOverviewLoader'
                anchors.fill: parent
                sourceComponent: allLoaded ? eventsOverviewComponent : loadingScreenComponent

                Component {
                    id: eventsOverviewComponent
                    EventsOverview {
                        id: eventsOverview
                        anchors.fill: parent

                        Connections {
                            target: photoViewerLoader.item
                            onCloseRequested: {
                                eventsOverview.head.visible = true;
                                eventsOverview.head.locked = false;
                            }
                        } 

                        onMediaSourcePressed: {
                            eventsOverview.head.visible = false;
                            eventsOverview.head.locked = true;
 
                            photoViewerLoader.load();
                            overview.mediaCurrentlyInView = mediaSource.path;

                            var rect = GalleryUtility.translateRect(thumbnailRect,
                                                                    eventsOverview,
                                                                    photoViewerLoader);
                            photoViewerLoader.item.title = eventTab.title;
                            photoViewerLoader.item.selection = selection;
                            photoViewerLoader.item.animateOpen(mediaSource, rect);
                        }

                        onInSelectionModeChanged: {
                            if (eventsOverview.inSelectionMode)
                                eventTab.title = i18n.tr("Select")
                            else
                                eventTab.title = i18n.tr("Events")
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
                anchors.fill: parent

                sourceComponent: allLoaded ? photosOverviewComponent : loadingScreenComponent

                Component {
                    id: photosOverviewComponent
                    PhotosOverview {
                        id: photosOverview
                        anchors.fill: parent
                        model: mediaLibraryLoader.item

                        Connections {
                            target: photoViewerLoader.item
                            onSelected: positionViewAtSelected(index);
                            onCloseRequested: {
                                photosOverview.head.visible = true;
                                photosOverview.head.locked = false;
                            }
                        } 

                        onMediaSourcePressed: {
                            photosOverview.head.visible = false;
                            photosOverview.head.locked = true;
 
                            photoViewerLoader.load();
                            overview.mediaCurrentlyInView = mediaSource.path;

                            var rect = GalleryUtility.translateRect(thumbnailRect,
                                                                    photosOverview,
                                                                    photoViewerLoader);
                            photoViewerLoader.item.title = photosTab.title;
                            photoViewerLoader.item.selection = selection;
                            photoViewerLoader.item.animateOpen(mediaSource, rect);
                        }

                        onInSelectionModeChanged: {
                            if (photosOverview.inSelectionMode)
                                photosTab.title = i18n.tr("Select")
                            else
                                photosTab.title = i18n.tr("Photos")
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

    /// Indicates if the photo viewer is currently open (shown to the user)
    property bool __isPhotoViewerOpen: photoViewerLoader.item && photoViewerLoader.item.isPoppedUp
    Loader {
        id: photoViewerLoader
        objectName: "photoViewerLoader"

        /// is true, if the photoviewer is currently loaded
        property bool loaded: photoViewerLoader.status === Loader.Ready

        function load() {
            setSource(Qt.resolvedUrl("MediaViewer/PopupPhotoViewer.qml"), {model: mediaLibraryLoader.item});
        }

        anchors.fill: parent
        z: 100


        Connections {
            target: photoViewerLoader.item
            onCloseRequested: {
                if (!APP.desktopMode)
                    setFullScreenAppMode(false);
                popPage();
                photoViewerLoader.item.fadeClosed();
                overview.mediaCurrentlyInView = "";
            }
        }
    }

    Loader {
        id: mediaLibraryLoader
        sourceComponent: allLoaded ? mediaLibraryComponent : ""        

        Component {
            id: mediaLibraryComponent
            MediaCollectionModel {
                id: mediaLibrary
                monitored: true
                onIndexAdded: {
                    if (APP.mediaFile == mediaLibrary.getAt(index).path)
                        openMediaFile(APP.mediaFile);
                }
            }
        }
    }

    MouseArea {
        id: blocker
        anchors.fill: parent
        enabled: photoViewerLoader.item && photoViewerLoader.item.animationRunning
    }

    UnityActions.ActionContext {
        id: hudCtx
        active: __isPhotoViewerOpen
    }
 
    actionManager.localContexts: [ hudCtx ]

    Connections {
        target: UriHandler
        onOpened: {
            for (var i = 0; i < uris.length; ++i) {
                APP.parseUri(uris[i])
            }
        }
    }

    Connections {
        target: APP
        onMediaFileChanged: {
            if (applicationLoaded && APP.mediaFile != "") {
                openMediaFile(APP.mediaFile);
            }
        }
    }

    Connections {
        target: ContentHub
        onExportRequested: {
            application.transfer = transfer
            APP.pickModeEnabled = true
        }
    }
}

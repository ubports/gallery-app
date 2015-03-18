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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Unity.Action 1.0 as UnityActions
import Gallery 1.0
import "../js/GalleryUtility.js" as GalleryUtility
import "AlbumEditor"
import "AlbumViewer"

MainView {
    id: overview
    objectName: "overview"

    useDeprecatedToolbar: false

    anchors.fill: parent
    applicationName: "com.ubuntu.gallery"
    automaticOrientation: application.automaticOrientation

    property string mediaCurrentlyInView
    StateSaver.properties: "mediaCurrentlyInView"

    property bool applicationLoaded: application.allLoaded

    //fullScreen property is used on autopilot tests
    property bool fullScreen: APP.fullScreen
   
    property alias currentPage: pageStack.currentPage

    function openMediaFile(media) {
        if (__isPhotoViewerOpen) {
            popPage();
            photoViewerLoader.item.fadeClosed();
        }

        mediaCurrentlyInView = media;
        for (var i = 0; i < MANAGER.mediaLibrary.count; i++) {
            if (MANAGER.mediaLibrary.getAt(i).path == mediaCurrentlyInView) {
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
                photoViewerLoader.item.animateOpen(MANAGER.mediaLibrary.getAt(i),
                                                   Qt.rect(0,0,0,0));
                return;
            }
        }
    }

    function setHeaderVisibility(visible, toggleFullscreen)
    {
        toggleFullscreen = typeof toggleFullscreen !== 'undefined' ? toggleFullscreen : true
        header.visible = visible;
        if (!APP.desktopMode && toggleFullscreen)
            setFullScreen(!visible);
    }

    function toggleHeaderVisibility()
    {
        setHeaderVisibility(!header.visible);
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
            if (__isPhotoViewerOpen) {
                photoViewerLoader.item.fadeClosed();
                popPage();
            } else if (allLoaded && albumsCheckerboardLoader.item) {
                albumsCheckerboardLoader.item.closeAlbum();
            }

            if (allLoaded && eventsOverviewLoader.item) {
                eventsOverviewLoader.item.positionViewAtBeginning();
            }

            setHeaderVisibility(true);

            tabs.selectedTabIndex = 1;
        }
    }

    function pushPage(page, properties) {
        return pageStack.push(page, properties);
    }

    function popPage() {
        pageStack.pop();
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

                        onMediaSourcePressed: {
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
                        model: MANAGER.mediaLibrary

                        Connections {
                            target: photoViewerLoader.item
                            onSelected: positionViewAtSelected(index);
                        } 

                        onMediaSourcePressed: {
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
            setSource(Qt.resolvedUrl("MediaViewer/PopupPhotoViewer.qml"), {model: MANAGER.mediaLibrary});
        }

        anchors.fill: parent
        z: 100


        Connections {
            target: photoViewerLoader.item
            onCloseRequested: {
                if (!APP.desktopMode)
                    setFullScreen(false);
                popPage();
                photoViewerLoader.item.fadeClosed();
                overview.mediaCurrentlyInView = "";
            }
        }
    }

    MouseArea {
        id: blocker
        anchors.fill: parent
        enabled: photoViewerLoader.item && photoViewerLoader.item.animationRunning
    }

    actionManager.localContexts: [
        UnityActions.ActionContext {
            id: hudCtx
            active: __isPhotoViewerOpen
        }
    ]

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
            if (applicationLoaded) {
                openMediaFile(APP.mediaFile);
            }
        }
    }
}

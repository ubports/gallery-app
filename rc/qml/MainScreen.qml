/*
 * Copyright (C) 2012 Canonical Ltd
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
import Gallery 1.0
import "../js/GalleryUtility.js" as GalleryUtility
import "Utility"

/*!
*/
MainView {
    id: overview
    objectName: "overview"

    anchors.fill: parent

    Tabs {
        id: tabs
        anchors.fill: parent
        Component.onCompleted: ItemStyle.style.swipeToSwitchTabs = false

        visible: !(photoViewerLoader.item && photoViewerLoader.item.isPoppedUp)
        active: visible && !albumViewer.isOpen && !albumEditor.isOpen

        selectedTabIndex: 1

        onSelectedTabIndexChanged: {
            if (selectedTabIndex == 0)
                albumsCheckerboardLoader.load();
            if (selectedTabIndex == 2)
                photosOverview.initModel()
            // prevent leaving views in selection mode
            eventView.leaveSelectionMode()
            photosOverview.leaveSelectionMode()
        }

        Tab {
            title: i18n.tr("Albums")
            page: Loader {
                id: albumsCheckerboardLoader
                objectName: "albumsCheckerboardLoader"
                anchors.fill: parent
                asynchronous: true

                function load() {
                    if (source == "")
                        source = "AlbumsOverview.qml"
                }
            }
        }

        Tab {
            id: eventTab
            title: i18n.tr("Events")
            page: OrganicEventView {
                id: eventView
                objectName: "organicEventView"

                anchors.fill: parent
                visible: true

                onMediaSourcePressed: {
                    photoViewerLoader.load();

                    var rect = GalleryUtility.translateRect(thumbnailRect, eventView, photoViewerLoader);
                    photoViewerLoader.item.animateOpen(mediaSource, rect);
                }

                // FIXME setting the title via a binding has wrong text placement at startup
                // so it is done here as a workaround.
                // The new implementation of the Tab header will hopefully fix this
                onInSelectionModeChanged: {
                    if (inSelectionMode)
                        eventTab.title = i18n.tr("Select")
                    else
                        eventTab.title = i18n.tr("Events")
                }
            }
        }

        // TODO: Although not using a Loader for the Photo Overview today
        // (see above TODO), will make sense in future when component becomes
        // more heavyweight and causes a longer startup time
        Tab {
            id: photosTab
            title: i18n.tr("Photos")
            objectName: "photosView"
            page: PhotosOverview {
                id: photosOverview

                anchors.fill: parent

                onMediaSourcePressed: {
                    photoViewerLoader.load();

                    var rect = GalleryUtility.translateRect(thumbnailRect,
                                                            photosOverview, photoViewerLoader);
                    photoViewerLoader.item.animateOpen(mediaSource, rect);
                }

                // FIXME setting the title via a binding has wrong text placement at startup
                // so it is done here as a workaround.
                // The new implementation of the Tab header will hopefully fix this
                onInSelectionModeChanged: {
                    if (inSelectionMode)
                        photosTab.title = i18n.tr("Select")
                    else
                        photosTab.title = i18n.tr("Photos")
                }
            }
        }
    }

    AlbumViewerAnimated {
        id: albumViewer
        objectName: "albumViewerAnimated"
        anchors.fill: parent
    }

    AlbumEditorAnimated {
        id: albumEditor
        objectName: "albumEditorAnimated"
        anchors.fill: parent
    }

    /// Indicates if the photo viewer is currently open (shown to the user)
    property bool __isPhotoViewerOpen: photoViewerLoader.item && photoViewerLoader.item.isPoppedUp
    Loader {
        id: photoViewerLoader
        objectName: "photoViewerLoader"

        /// is true, if the photoviewer is currently loaded
        property bool loaded: photoViewerLoader.status === Loader.Ready

        function load() {
            if (!sourceComponent)
                sourceComponent = photoViewerComponent;
        }

        anchors.fill: parent
        z: 100

        Component {
            id: photoViewerComponent

            PopupPhotoViewer {
                id: popupPhotoViewer

                model: MediaCollectionModel {
                    monitored: true
                }

                onCloseRequested: fadeClosed()
            }
        }
    }

    MouseArea {
        id: blocker
        anchors.fill: parent
        enabled: photoViewerLoader.item && photoViewerLoader.item.animationRunning
    }
}

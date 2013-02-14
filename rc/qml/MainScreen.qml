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

    tools: __isPhotoViewerOpen ? photoViewerLoader.item.tools
            : (albumViewer.isOpen ? albumViewer.tools
            : tabs.tools)

    Tabs {
        id: tabs
        anchors.fill: parent
        ItemStyle.class: "new-tabs"
        Component.onCompleted: ItemStyle.style.swipeToSwitchTabs = false

        visible: !(photoViewerLoader.item && photoViewerLoader.item.isPoppedUp)

        selectedTabIndex: 1

        onSelectedTabIndexChanged: {
            if (selectedTabIndex == 0)
                albumsCheckerboardLoader.load();
            if (selectedTabIndex == 2)
                photosOverview.initModel()
            // prevent leaving the event view in selection mode
            eventView.leaveSelectionMode()
        }

        // TODO: Loaders don't play well with Tabs, they prevent the tab bar
        // from sliding upward when scrolling:
        Tab {
            title: "Albums"
            page: Loader {
                id: albumsCheckerboardLoader
                objectName: "albumsCheckerboardLoader"
                anchors.fill: parent
                asynchronous: true
                property ToolbarActions tools: status === Loader.Ready ? item.tools : null

                function load() {
                    if (source == "")
                        source = "AlbumsOverview.qml"
                }
            }
        }

        Tab {
            id: eventTab
            title: "Events"
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
                        eventTab.title = "Select"
                    else
                        eventTab.title = "Events"
                }
            }
        }

        // TODO: Although not using a Loader for the Photo Overview today
        // (see above TODO), will make sense in future when component becomes
        // more heavyweight and causes a longer startup time
        Tab {
          title: "Photos"
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
          }
        }
    }

    AlbumViewerAnimated {
        id: albumViewer
        anchors.fill: parent
    }

    AlbumEditorAnimated {
        id: albumEditor
        anchors.fill: parent
    }

    /// Indicates if the photo viewer is currently open (shown to the user)
    property bool __isPhotoViewerOpen: photoViewerLoader.item && photoViewerLoader.item.isPoppedUp
    Loader {
        id: photoViewerLoader

        anchors.fill: parent
        z: 100

        function load() {
            if (!sourceComponent)
                sourceComponent = photoViewerComponent;
        }

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

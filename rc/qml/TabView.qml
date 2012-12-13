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
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import "../Capetown"
import "../Capetown/Widgets"
import "../js/GalleryUtility.js" as GalleryUtility
import "Components"
import "Utility"
import "Widgets"

Rectangle {
    id: overview
    objectName: "overview"

    signal albumSelected(variant album, variant thumbnailRect)

    property Rectangle glass: overviewGlass

    anchors.fill: parent

    function getRectOfAlbumPreview(album, relativeTo) {
        albumsCheckerboardLoader.load();

        var checkerboard = albumsCheckerboardLoader.item;
        return checkerboard.getRectOfItemAt(checkerboard.model.indexOf(album), relativeTo);
    }

    function showAlbumPreview(album, show) {
        albumsCheckerboardLoader.load();

        var checkerboard = albumsCheckerboardLoader.item;
        var delegate = checkerboard.getDelegateInstanceAt(checkerboard.model.indexOf(album));
        if (delegate)
            delegate.visible = show;
    }

    Tabs {
        id: tabs

        anchors.fill: parent
        ItemStyle.class: "new-tabs"
        Component.onCompleted: ItemStyle.style.swipeToSwitchTabs = false

        selectedTabIndex: 1
        onSelectedTabIndexChanged: {
          if (selectedTabIndex == 0)
            albumsCheckerboardLoader.load();
        }

        // TODO: Loaders don't play well with Tabs, they prevent the tab bar
        // from sliding upward when scrolling:
        // https://bugs.launchpad.net/goodhope/+bug/1088740
        Tab {
            title: "Albums"
            page: Loader {
                id: albumsCheckerboardLoader
                objectName: "albumsCheckerboardLoader"

                anchors.fill: parent

                function load() {
                    if (!sourceComponent)
                        sourceComponent = albumsCheckerboardComponent;
                }

                Component {
                    id: albumsCheckerboardComponent

                    Checkerboard {
                        id: albumsCheckerboard

                        topExtraGutter: getDeviceSpecific("albumGridTopMargin")
                        bottomExtraGutter: getDeviceSpecific("albumGridGutterHeight") / 2
                        leftExtraGutter: getDeviceSpecific("albumGridLeftMargin")
                        rightExtraGutter: getDeviceSpecific("albumGridRightMargin")

                        itemWidth: getDeviceSpecific("albumThumbnailWidth")
                        itemHeight: getDeviceSpecific("albumThumbnailHeight")
                        minGutterWidth: getDeviceSpecific("albumGridGutterWidth")
                        minGutterHeight: getDeviceSpecific("albumGridGutterHeight")

                        visible: albumsCheckerboardLoader.visible

                        selection: SelectionState {
                            allowSelectionModeChange: false
                            model: albumsCheckerboard.model
                        }

                        model: AlbumCollectionModel {
                        }

                        delegate: CheckerboardDelegate {
                            property real commitFraction: 0.05

                            // internal
                            property bool validSwipe: false

                            z: (albumThumbnail.isFlipping ? 10 : 0)

                            checkerboard: albumsCheckerboard

                            contentIsSwipable: album.containedCount > 0

                            onSwipeStarted: {
                                validSwipe = ((leftToRight && !album.closed) || (!leftToRight && album.closed));
                            }

                            onSwiping: {
                                if (!validSwipe)
                                    return;

                                var availableDistance = (leftToRight) ? (width - start) : start;
                                var fraction = Math.max(0, Math.min(1, distance / availableDistance));

                                albumThumbnail.openFraction = (leftToRight ? 1 - fraction : fraction);
                            }

                            onLongPressed: {
                                albumMenu.show(album)
                            }

                            onSwiped: {
                                if (!validSwipe)
                                    return;

                                var fraction = (leftToRight
                                                ? 1 - albumThumbnail.openFraction
                                                : albumThumbnail.openFraction);
                                if ((leftToRight && fraction >= commitFraction)
                                        || (!leftToRight && fraction < commitFraction))
                                    albumThumbnail.close();
                                else
                                    albumThumbnail.open();
                            }

                            content: AlbumPreviewComponent {
                                id: albumThumbnail

                                property real maxAddScale: 0.5

                                width: albumsCheckerboard.itemWidth
                                height: albumsCheckerboard.itemHeight

                                album: modelData.album
                                load: true

                                // Scale from 1 to 1 + maxAddScale and back to 1 as openFraction goes
                                // from 0 to 0.5 to 1.
                                scale: 1 + maxAddScale - Math.abs((openFraction - 0.5) * maxAddScale * 2)
                            }
                        }

                        onActivated: {
                            var albumRect = GalleryUtility.translateRect(activatedRect, albumsCheckerboard, overview);
                            albumSelected(object, albumRect);
                        }
                    }
                }
            }
        }

        Tab {
            title: "Events"
            page: OrganicEventView {
                id: eventView

                anchors.fill: parent
                visible: true

                onMediaSourcePressed: {
                    photoViewerLoader.load();

                    var rect = GalleryUtility.translateRect(thumbnailRect, eventView, photoViewerLoader);
                    photoViewerLoader.item.animateOpen(mediaSource, rect);
                }
            }
        }
        
        // TODO: Although not using a Loader for the Photo Overview today
        // (see above TODO), will make sense in future when component becomes
        // more heavyweight and causes a longer startup time
        Tab {
          title: "Photos"
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

    Rectangle {
        id: overviewGlass

        anchors.fill: parent

        color: "black"
        opacity: 0.0
    }

    AlbumEditMenu {
        id: albumMenu

        visible: false
        state: "hidden"

        property Album album

        function show(a) {
            album = a;
            var rect = getRectOfAlbumPreview(album, overview);
            if (rect.x <= overview.width / 2)
                popupOriginX = rect.x + rect.width + units.gu(4);
            else
                popupOriginX = rect.x - childrenRect.width;

            popupOriginY = rect.y >= units.gu(6) ? rect.y : units.gu(6);
            state = "shown"
        }

        onActionInvoked: {
            // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you
            // edit a switch statement in QML.  The short version is: use braces
            // always.
            switch (name) {
            case "onEdit": {
                albumEditor.editAlbum(album);

                showAlbumPreview(album, false);
                var thumbnailRect = getRectOfAlbumPreview(album, overview);
                albumEditorTransition.enterEditor(album, thumbnailRect);
                break;
            }

            case "onExport": {
                // TODO
                break;
            }

            case "onPrint": {
                // TODO
                break;
            }

            case "onShare": {

                for (var index = 0; index < album.allMediaSources.length; index++) {
                    shareImage(album.allMediaSources[index]);
                }
                break;
            }

            case "onDelete": {
                albumTrashDialog.show(album)
                break;
            }
            }
        }

        onPopupInteractionCompleted: state = "hidden"
    }

    // Dialog for deleting albums.
    DeleteOrDeleteWithContentsDialog {
        id: albumTrashDialog

        property variant album: null

        visible: false

        deleteTitle: "Delete album"
        deleteWithContentsTitle: "Delete album + contents"

        function show(albumToShow) {
            album = albumToShow;
            state = "shown"

            var rect = getRectOfAlbumPreview(album, overview);
            if (rect.x <= overview.width / 2)
                popupOriginX = rect.x + rect.width + units.gu(4);
            else
                popupOriginX = rect.x - childrenRect.width;

            popupOriginY = rect.y >= units.gu(6) ? rect.y : units.gu(6);
        }

        onDeleteRequested: {
            albumsCheckerboardLoader.load();
            albumsCheckerboardLoader.item.model.destroyAlbum(album)
        }

        onDeleteWithContentsRequested: {
            // Remove contents.
            var list = album.allMediaSources;
            for (var i = 0; i < list.length; i++)
                eventView.selection.model.destroyMedia(list[i]);

            // Remove album.
            albumsCheckerboardLoader.load();
            albumsCheckerboardLoader.item.model.destroyAlbum(album);
        }

        onPopupInteractionCompleted: state = "hidden"
    }

    // Cancel out of menus if user clicks outside the menu area.
    MouseArea {
        id: menuCancelArea

        anchors.fill: parent
        visible: (albumMenu.state === "shown" || albumTrashDialog.state === "shown")
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            albumMenu.state = "hidden";
            albumTrashDialog.state = "hidden";
        }
    }

    AlbumEditor {
        id: albumEditor

        anchors.fill: parent

        visible: false

        onMediaSelectorHidden: {
            albumEditorCheckerboardHidden(newScrollPos);
        }

        onCloseRequested: {
            if (album) {
                if (enterViewer) {
                    overview.albumSelected(album, null);
                    albumEditorTransition.exitEditor(null, null);
                } else {
                    var thumbnailRect = overview.getRectOfAlbumPreview(album, albumEditorTransition);

                    overview.showAlbumPreview(album, false);
                    albumEditorTransition.exitEditor(album, thumbnailRect);
                }
            } else {
                albumEditorTransition.exitEditor(null, null);
            }
        }
    }

    AlbumEditorTransition {
        id: albumEditorTransition

        anchors.fill: parent

        backgroundGlass: overview.glass
        editor: albumEditor

        onEditorExited: overview.showAlbumPreview(album, true)
    }

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

                onCloseRequested: {
                  fadeClosed()
                  tabs.visible = true;
                }

                onOpened: tabs.visible = false;
            }
        }
    }

    MouseArea {
        id: blocker

        anchors.fill: parent

        visible: (photoViewerLoader.item && photoViewerLoader.item.animationRunning) ||
                 albumEditorTransition.animationRunning || albumEditor.animationRunning
    }
}

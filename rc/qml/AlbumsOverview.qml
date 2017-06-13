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
 *
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Gallery 1.0
import "AlbumEditor"
import "AlbumViewer"
import "Components"
import "Utility"

/*!
  Shows all the albums in one grid view
 */
Checkerboard {
    id: root

    property bool isAlbumOpened: false

    property int albumCurrentlyInView: -1
    StateSaver.properties: "albumCurrentlyInView"

    Component.onCompleted: {
        /* FIXME: Does not working after PageStack
        if (albumCurrentlyInView != -1) {
            for (var i = 0; i < albumCollectionModel.count; i++) {
                if (albumCollectionModel.getAt(i).id == albumCurrentlyInView) {
                    if (albumCollectionModel.getAt(i).currentPage < 0)
                        albumCollectionModel.getAt(i).currentPage = 1

                    albumViewer.album = albumCollectionModel.getAt(i);
                    root.visible = false;

                    if (albumViewer.origin) {
                        albumViewer.visible = true
                        overview.pushPage(albumViewer);
                    }
                    else
                        albumViewer.visible = true

                    if (albumViewer.previewItem)
                        albumViewer.previewItem.visible = false

                    return;
                }
            }
        }
        */
    }

    onActiveChanged: {
        if (active) {
            root.visible = true;
        }
    }

    /*!
    */
    function getRectOfAlbumPreview(album, relativeTo) {
        return getRectOfItemAt(model.indexOf(album), relativeTo);
    }

    function closeAlbum() {
        if (isAlbumOpened)
            albumViewer.closeAlbum();
    }

    topExtraGutter: getDeviceSpecific("albumGridTopMargin")
    bottomExtraGutter: getDeviceSpecific("albumGridGutterHeight") / 2
    leftExtraGutter: getDeviceSpecific("albumGridLeftMargin")
    rightExtraGutter: getDeviceSpecific("albumGridRightMargin")

    itemWidth: getDeviceSpecific("albumThumbnailWidth")
    itemHeight: getDeviceSpecific("albumThumbnailHeight")
    minGutterWidth: getDeviceSpecific("albumGridGutterWidth")
    minGutterHeight: getDeviceSpecific("albumGridGutterHeight")

    selection: SelectionState {
        allowSelectionModeChange: false
        model: root.model
    }

    model: AlbumCollectionModel {
        id: albumCollectionModel
    }

    delegate: CheckerboardDelegate {
        id: thisDelegate
        property real commitFraction: 0.05

        // internal
        property bool validSwipe: false

        z: (albumThumbnail.isFlipping ? 10 : 0)

        checkerboard: root

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
            albumMenu.album = album
            albumMenu.caller = content
            albumMenu.show()
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

            width: root.itemWidth
            height: root.itemHeight

            album: modelData.album
            load: true

            // Scale from 1 to 1 + maxAddScale and back to 1 as openFraction goes
            // from 0 to 0.5 to 1.
            scale: 1 + maxAddScale - Math.abs((openFraction - 0.5) * maxAddScale * 2)
        }
    }

    onActivated: {
        if (object.currentPage < 0)
            object.currentPage = 1

        isAlbumOpened = true
        albumCurrentlyInView = object.id
        albumViewer.album = object
        albumViewer.origin = root.getRectOfAlbumPreview(object, albumViewer)
        albumViewer.previewItem = activatingItem
        root.visible = false;

        if (albumViewer.origin) {
            albumViewer.visible = true;
            if (!APP.desktopMode)
                setFullScreenAppMode(true);
            overview.pushPage(albumViewer);
            if (object.containedCount > 0)
                albumViewer.header.visible = false;
            else
                albumViewer.header.visible = true;
        }
        else
            albumViewer.visible = true

        if (albumViewer.previewItem)
            albumViewer.previewItem.visible = false
    }

    Rectangle {
        id: overviewGlass

        anchors.fill: parent

        color: "black"
        opacity: 0.0
    }

    AlbumEditMenu {
        id: albumMenu

        property Album album

        visible: false

        onEditClicked: albumEditorLoader.load(album)

        onDeleteClicked: {
            albumTrashDialog.album = album
            albumTrashDialog.caller = caller
            albumTrashDialog.show()
        }
    }

    // Dialog for deleting albums.
    DeleteOrDeleteWithContentsDialog {
        id: albumTrashDialog

        visible: false
    }

    /// Contains the actions for the toolbar in the albums tab
    head.actions: [
        Action {
            objectName: "addButton"
            text: i18n.tr("Add new album") // Text in HUD
            iconName: "add"
            onTriggered: {
                var album = albumCollectionModel.createOrphan();
                album.title = i18n.tr("New Photo Album");
                album.subtitle = i18n.tr("Subtitle");

                albumEditorLoader.load(album)
            }
        },
        Action {
            objectName: "cameraButton"
            text: i18n.tr("Camera")
            visible: !APP.desktopMode
            iconName: "camera-app-symbolic"
            onTriggered: Qt.openUrlExternally("appid://com.ubuntu.camera/camera/current-user-version")
        }
    ]

    AlbumViewer {
        id: albumViewer
        objectName: "albumViewer"
        anchors.fill: parent
        visible: false
        onIsOpenChanged: if (!isOpen) albumCurrentlyInView = -1

        onCloseRequested: isAlbumOpened = false;
        onQuickCloseRequested: isAlbumOpened = false;
    }

    Loader {
        id: albumEditorLoader
        anchors.fill: parent

        property var album

        onStatusChanged: {
            if (status === Loader.Ready) {
                albumEditorLoader.item.album = albumEditorLoader.album
                overview.pushPage(albumEditorLoader.item)
            }
        }

        function load(album) {
            albumEditorLoader.album = album
            sourceComponent = albumEditorComponent
        }

        Component {
            id: albumEditorComponent
            AlbumEditor {
                id: albumEditor
                objectName: "albumEditor"
                anchors.fill: parent
                visible: false 

                onMediaSelectorHidden: albumEditorCheckerboardHidden(newScrollPos);
                onCloseRequested: {
                    overview.popPage()
                    albumEditorLoader.album = null
                    albumEditorLoader.sourceComponent = null
                }
            }
        }
    }
}

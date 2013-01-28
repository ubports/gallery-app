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
import Gallery 1.0
import "Components"
import "Utility"
import "Widgets"

/*!
  Shows all the albums in one grid view
 */
Checkerboard {
    id: root

    /*!
    */
    function getRectOfAlbumPreview(album, relativeTo) {
        return getRectOfItemAt(model.indexOf(album), relativeTo);
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
    }

    delegate: CheckerboardDelegate {
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
        albumViewer.album = object
        albumViewer.origin = root.getRectOfAlbumPreview(object, albumViewer)
        albumViewer.open()
    }

    Rectangle {
        id: overviewGlass

        anchors.fill: parent

        color: "black"
        opacity: 0.0
    }

    // Cancel out of menus if user clicks outside the menu area.
    MouseArea {
        id: menuCancelArea

        anchors.fill: parent
        visible: albumMenu.state === "shown" || albumTrashDialog.state === "shown"
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            albumMenu.state = "hidden";
            albumTrashDialog.state = "hidden";
        }
    }

    AlbumEditMenu {
        id: albumMenu

        visible: false
        state: "hidden"

        property Album album

        function show(a) {
            album = a;
            var rect = root.getRectOfAlbumPreview(album, root);
            if (rect.x <= root.width / 2)
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
                albumEditor.album = album
                albumEditor.origin = root.getRectOfAlbumPreview(album, albumEditor)
                albumEditor.open()
                break;
            }
            case "onExport": break // TODO
            case "onPrint": break // TODO
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

            var rect = root.getRectOfAlbumPreview(album, root);
            if (rect.x <= root.width / 2)
                popupOriginX = rect.x + rect.width + units.gu(4);
            else
                popupOriginX = rect.x - childrenRect.width;

            popupOriginY = rect.y >= units.gu(6) ? rect.y : units.gu(6);
        }

        property AlbumCollectionModel albumCollection: AlbumCollectionModel {}
        property MediaCollectionModel mediaCollection: MediaCollectionModel {}

        onDeleteRequested: {
            albumCollection.destroyAlbum(album)
        }

        onDeleteWithContentsRequested: {
            // Remove contents.
            var list = album.allMediaSources;
            for (var i = 0; i < list.length; i++)
                mediaCollection.destroyMedia(list[i]);

            // Remove album.
            albumCollection.destroyAlbum(album);
        }

        onPopupInteractionCompleted: state = "hidden"
    }
}

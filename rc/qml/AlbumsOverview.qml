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
import Ubuntu.Components 0.1
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

    property int lastOpenedAlbum: -1
    StateSaver.properties: "lastOpenedAlbum"

    Component.onCompleted: {
        if (lastOpenedAlbum != -1) {
            var target = lastOpenedAlbum;
            lastOpenedAlbum = -1;
            for (var i = 0; i < albumCollectionModel.count; i++) {
                if (albumCollectionModel.getAt(i).id == target) {
                    albumViewer.album = albumCollectionModel.getAt(i);
// TODO: find the delegate, scroll the list to it, then pass it on when opening the album
// TODO: this is necessary for the opening animation but for the "back" animation.
// TODO: In fact the opening animation shouldn't happen at all
//                    albumViewer.origin = root.getRectOfAlbumPreview(object, albumViewer)
//                    albumViewer.previewItem = activatingItem
                    albumViewer.open();
                    return;
                }
            }
        }
    }

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
            albumEditor.previewItem = thisDelegate
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
        lastOpenedAlbum = object.id
        albumViewer.album = object
        albumViewer.origin = root.getRectOfAlbumPreview(object, albumViewer)
        albumViewer.previewItem = activatingItem
        albumViewer.open()
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

        onEditClicked: {
            albumEditor.album = album
            albumEditor.origin = root.getRectOfAlbumPreview(album, albumEditor)
            albumEditor.open()
        }

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
    tools: ToolbarItems {
        id: albumOverviewTools
        ToolbarButton {
            objectName: "addButton"
            action: Action {
                text: i18n.tr("Add new album") // Text in HUD
                iconSource: Qt.resolvedUrl("../img/add.png")
                onTriggered: {
                    var album = albumCollectionModel.createOrphan();
                    albumCollectionModel.addOrphan(album);

                    albumEditor.album = album;
                    albumEditor.origin = null;
                    albumEditor.previewItem = null;
                    albumEditor.open();
                }
            }
            text: "Add" // text in toolbar
        }
        ToolbarButton {
            objectName: "cameraButton"
            action: Action {
                text: i18n.tr("Camera")
                iconSource: Qt.resolvedUrl("../img/camera.png")
                onTriggered: Qt.openUrlExternally("application:///camera-app.desktop")
            }
        }
    }
}

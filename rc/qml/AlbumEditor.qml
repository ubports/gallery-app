/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import "../Capetown"
import "../js/GalleryUtility.js" as GalleryUtility
import "../js/GraphicsRoutines.js" as GraphicsRoutines
import "Components"
import "Widgets"

/*!
*/
Page {
    id: albumEditor
    objectName: "mainAlbumEditor"

    title: "Edit album"
    tools: ToolbarActions {
        back: Action {
            text: "cancel"
            iconSource: Qt.resolvedUrl("../img/cancel.png")
            onTriggered: {
                    albumEditor.closeRequested(albumEditor.album, false);
            }
        }
    }

    /*!
    */
    signal closeRequested(variant album, bool enterViewer)
    /*!
    */
    signal mediaSelectorHidden(int newScrollPos)

    /*!
    */
    property Album album
    /*!
    */
    property real minimumCoverWidth: units.gu(32)
    /*!
    */
    property real minimumCoverHeight: units.gu(38)
    /*!
    */
    property real preferredCoverWidth: width - units.gu(8)
    /*!
    */
    property real preferredCoverHeight: height - units.gu(8)
    /*!
    */
    property real minimumTopMargin: units.gu(3)

    // readonly
    /*!
    */
    property variant editorRect
    /*!
    */
    property alias animationRunning: mediaSelector.animationRunning

    // internal
    /*!
    */
    property real canonicalWidth: units.gu(66)
    /*!
    */
    property real canonicalHeight: units.gu(80)

    /*!
    */
    function editNewAlbum() {
        albumEditor.album = albumModel.createOrphan();
        coverMenu.state = "hidden"
    }

    /*!
    */
    function editAlbum(album) {
        albumEditor.album = album;
        coverMenu.state = "hidden"
    }

    /*!
    */
    function setMediaSelectorScrollPos(newScrollPos) {
        mediaSelector.setCheckerboardScrollPos(newScrollPos);
    }

    // internal
    /*!
    */
    function closeAlbum() {
        if (album.populatedContentPageCount > 0) {
            albumModel.addOrphan(album);

            // Don't want to stay on the cover.
            if (album.currentPage == album.firstValidCurrentPage)
                album.currentPage = album.firstContentPage;
        } else {
            albumModel.destroyOrphan(album);
        }
    }

    // internal
    /*!
    */
    function resetEditorRect() {
        editorRect = GalleryUtility.getRectRelativeTo(cover.internalRect, albumEditor);
    }

    onAlbumChanged: resetEditorRect() // HACK: works, but not conceptually correct.
    onWidthChanged: resetEditorRect()
    onHeightChanged: resetEditorRect()

    AlbumCollectionModel {
        id: albumModel
    }

    MouseArea {
        id: coverCloser

        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        onPressed: {
            coverMenu.state = "hidden";
            cover.editingDone();
            closeAlbum();

            albumEditor.closeRequested(albumEditor.album, false);
        }
    }

    AspectArea {
        id: coverArea

        x: (parent.width - width) / 2
        y: Math.max((parent.height - height) / 2, minimumTopMargin)

        width: GraphicsRoutines.clamp(
                   preferredCoverWidth, minimumCoverWidth, canonicalWidth)
        height: GraphicsRoutines.clamp(
                    preferredCoverHeight, minimumCoverHeight, canonicalHeight)

        aspectWidth: canonicalWidth
        aspectHeight: canonicalHeight

        content: AlbumCover {
            id: cover

            anchors.fill: parent

            album: albumEditor.album
            isPreview: false

            onPressed: {
                mouse.accepted = true;
                if (!isTextEditing) {
                    coverMenu.flipVisibility();
                } else {
                    cover.editingDone()
                }
            }

            onIsTextEditingChanged: {
                // Hide menu when we start editing text.
                if (isTextEditing && coverMenu.state !== "hidden")
                    coverMenu.state = "hidden";
            }

            onAddPhotos: mediaSelector.show();
        }
    }

    // Cover picker
    AlbumCoverMenu {
        id: coverMenu

        visible: false
        state: "hidden"
        popupOriginX: -units.gu(3)
        popupOriginY: -units.gu(15)

        onActionInvoked: {
            albumEditor.album.coverNickname = name
            state = "hidden"
        }
    }

    MediaSelector {
        id: mediaSelector

        onAddClicked: {
            var album  = albumEditor.album;
            var firstPhoto = album.addSelectedMediaSources(selection.model);
        }

        onHidden: {
            albumEditor.closeRequested(albumEditor.album, true);
        }
    }
}

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
import Ubuntu.Components 0.1

/*! @brief AlbumEditorAnimated opens the editor for the album
  *
  * The album editor enable to edit the title, sub-title and the cover image.
  * Set the album and origin properties befor calling open()
  * The first call of open() is slow, as the lazy loading is used for the album editor which is a
  * big component.
  */
Item {
    id: root

    // True if the album editor is opened
    property bool isOpen: false
    /// The album to be shown in that editor
    property Album album
    /// Origin (rectangle) where this view is animated from when calling open()
    /// And where it is animated to when this view is closed
    property variant origin
    /// The preview item that was used to open the view, or null, if it was opend another way
    /// This item will be hidden when this view is opened. And shown again after closing this view
    property Item previewItem: null
    /// Is true if the opne or close animation is running
    property bool animationRunning: loader_albumEditor.status === Loader.Ready ?
                                        loader_albumEditor.item.albumEditorTransition.animationRunning ||
                                        loader_albumEditor.item.albumEditor.animationRunning
                                      : false

    /// Opens the editor for the album with an animation
    function open() {
        loader_albumEditor.load()
        loader_albumEditor.item.albumEditor.album = album
        loader_albumEditor.item.albumEditorTransition.enterEditor(root.album, root.origin)
        if (previewItem)
            previewItem.visible = false
        isOpen = true;
    }

    Component {
        id: component_EditorItem
        Item {
            property alias albumEditorTransition: inner_albumEditorTransition
            property alias albumEditor: inner_albumEditor

            Rectangle {
                id: overviewGlass
                anchors.fill: parent
                color: "black"
                opacity: 0.0
            }

            AlbumEditor {
                id: inner_albumEditor
                anchors.fill: parent
                visible: false
                active: root.isOpen

                onMediaSelectorHidden: {
                    albumEditorCheckerboardHidden(newScrollPos);
                }

                onCloseRequested: {
                    root.isOpen = false;
                    if (album) {
                        inner_albumEditorTransition.exitEditor(album, root.origin);
                    } else {
                        inner_albumEditorTransition.exitEditor(null, null);
                    }
                }
            }

            AlbumEditorTransition {
                id: inner_albumEditorTransition
                anchors.fill: parent
                backgroundGlass: overviewGlass
                editor: inner_albumEditor
                onEditorExited: {
                    if (previewItem)
                        previewItem.visible = true;
                    loader_albumEditor.sourceComponent = undefined;
                }
            }
        }
    }
    Loader {
        id: loader_albumEditor
        anchors.fill: parent

        function load() {
            if (sourceComponent == undefined) {
                sourceComponent = component_EditorItem
            }
        }
    }

    MouseArea {
        id: transitionClickBlocker
        anchors.fill: parent
        enabled: root.animationRunning
    }
}

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

/*! @brief AlbumEditorAnimated opens the editor for the album
  *
  * The album editor enable to edit the title, sub-title and the cover image.
  * Set the album and origin properties befor calling open()
  * The first call of open() is slow, as the lazy loading is used for the album editor which is a
  * big component.
  */
Item {
    id: root

    /// The album to be shown in that editor
    property Album album
    /// Origin (rectangle) where this view is animated from when calling open()
    /// And where it is animated to when this view is closed
    property variant origin
    /// Is true if the opne or close animation is running
    property bool animationRunning: loader_albumEditor.status === Loader.Ready ?
                                        loader_albumEditor.item.albumEditorTransition.animationRunning ||
                                        loader_albumEditor.item.albumEditor.animationRunning
                                      : false

    /// Opens the editor for the album with an animation
    function open() {
        loader_albumEditor.load()
        loader_albumEditor.item.albumEditor.album = album
        if (root.origin)
            loader_albumEditor.item.albumEditorTransition.enterEditor(root.album, root.origin)
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

                onMediaSelectorHidden: {
                    albumEditorCheckerboardHidden(newScrollPos);
                }

                onCloseRequested: {
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

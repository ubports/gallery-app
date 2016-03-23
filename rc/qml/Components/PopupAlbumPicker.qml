/*
 * Copyright (C) 2011-2015 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.4
import Gallery 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import "../AlbumViewer"
import "../../js/Gallery.js" as Gallery

/*!
  Show a popover with all exisitng albums. With the option to click on one of them.
*/
Popover {
    id: popupAlbumPicker
    objectName: "popupAlbumPicker"

    /// Emitted when an album is clicked
    signal albumPicked(variant album)
    /// Emitted when a new album was created to add the photo(s) to
    signal newAlbumPicked(variant album)

    /// height of the content
    property int contentHeight: -1
    contentWidth: units.gu(34)

    visible: false

    Component {
        id: component_Item
        Item {
            clip: true

            Flickable {
                anchors.top: titleBox.bottom
                anchors.topMargin: units.gu(1)
                anchors.bottom: parent.bottom
                anchors.bottomMargin: units.gu(0.25)
                anchors.horizontalCenter: parent.horizontalCenter
                width: albumGrid.width

                contentHeight: albumGrid.height

                Grid {
                    id: albumGrid

                    property int albumPreviewWidth: units.gu(14);
                    property int albumPreviewHeight: units.gu(16.5);
                    property int gutterWidth: units.gu(2)
                    property int gutterHeight: units.gu(4)
                    property int cellWidth: albumPreviewWidth + gutterWidth
                    property int cellHeight: albumPreviewHeight + gutterHeight
                    property AlbumCollectionModel albumCollectionModel: AlbumCollectionModel{}

                    columns: Math.floor((popupAlbumPicker.contentWidth - units.gu(2)) / cellWidth)
                    rows: Math.ceil((albumCollectionModel.count + 1) / columns)

                    Item {
                        width: albumGrid.cellWidth
                        height: albumGrid.cellHeight
                        UbuntuShape {
                            width: albumGrid.albumPreviewWidth
                            height: albumGrid.albumPreviewHeight
                            anchors.centerIn: parent
                            color: "lightgray"

                            Label {
                                anchors.top: parent.top
                                anchors.topMargin: units.gu(3)
                                anchors.left: parent.left
                                anchors.leftMargin: units.gu(3)
                                anchors.right: parent.right
                                anchors.rightMargin: units.gu(3)
                                text: i18n.tr("Add new album")
                                fontSize: "medium"
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    var album = albumGrid.albumCollectionModel.createOrphan();
                                    album.title = i18n.tr("New Photo Album");
                                    album.subtitle = i18n.tr("Subtitle");
                                    album.newAlbum = false;
                                    albumGrid.albumCollectionModel.addOrphan(album);

                                    popupAlbumPicker.hide();
                                    popupAlbumPicker.albumPicked(album);
                                    popupAlbumPicker.newAlbumPicked(album);

                                    albumEditor.album = album;
                                    albumEditor.origin = null;
                                    albumEditor.previewItem = null;
                                    albumEditor.open();
                                }
                            }
                        }
                    }

                    Repeater {
                        model: albumGrid.albumCollectionModel

                        Item {
                            width: albumGrid.cellWidth
                            height: albumGrid.cellHeight

                            AlbumPreviewComponent {
                                width: albumGrid.albumPreviewWidth
                                height: albumGrid.albumPreviewHeight
                                anchors.centerIn: parent

                                album: model.album
                                showClosed: true

                                MouseArea {
                                    anchors.fill: parent

                                    onClicked: {
                                        popupAlbumPicker.hide();
                                        popupAlbumPicker.albumPicked(album);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: titleBox
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: units.gu(3)
                color: "#dddddd"

                Label {
                    id: title
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "black"
                    text: i18n.tr("Add Photo to Album")
                }
            }
        }
    }
    Loader {
        id: loader_Item

        width: parent.width
        height: contentHeight
        asynchronous: true

        sourceComponent: popupAlbumPicker.visible ? component_Item : undefined
    }
}

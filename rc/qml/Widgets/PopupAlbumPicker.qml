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
 *
 * Authors:
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import "../Components"
import "../../js/Gallery.js" as Gallery

/*!
  Show a popover with all exisitng albums. With the option to click on one of them.
*/
Popover {
    id: popupAlbumPicker

    /// Emitted when an album is clicked
    signal albumPicked(variant album);
    /// height of the content
    property int contentHeight: -1

    visible: false

    Component {
        id: component_Item
        Item {
            GridView {
                id: scroller

                property int albumPreviewWidth: units.gu(14);
                property int albumPreviewHeight: units.gu(16.5);
                property int gutterWidth: units.gu(2)
                property int gutterHeight: units.gu(4)

                clip: true
                anchors.top: titleBox.bottom
                anchors.bottom: parent.bottom
                anchors.bottomMargin: units.gu(0.25)
                anchors.left: parent.left
                anchors.leftMargin: units.gu(4)
                anchors.right: parent.right

                cellWidth: scroller.albumPreviewWidth + scroller.gutterWidth
                cellHeight: scroller.albumPreviewHeight + scroller.gutterHeight

                header: Item {
                    width: parent.width
                    height: units.gu(2);
                }
                footer: Item {
                    width: parent.width
                    height: scroller.gutterHeight / 2
                }

                model: AlbumCollectionModel {
                }

                delegate: Item {
                    width: scroller.cellWidth
                    height: scroller.cellHeight

                    AlbumPreviewComponent {
                        album: model.album

                        width: scroller.albumPreviewWidth
                        height: scroller.albumPreviewHeight
                        anchors.centerIn: parent

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                popupAlbumPicker.hide()
                                popupAlbumPicker.albumPicked(album);
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
                    font.family: "Ubuntu"
                    text: "Add Photo to Album"
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

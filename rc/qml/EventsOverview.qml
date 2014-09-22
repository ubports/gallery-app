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
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Gallery 1.0
import Ubuntu.Content 0.1
import "Components"
import "OrganicView"
import "Utility"
import "../js/GalleryUtility.js" as GalleryUtility

// An "organic" vertically-scrollable view of all events, each containing a
// horizontally-scrollable "tray" of photos.
OrganicView {
    id: organicEventView

    /// True if in the selection mode
    property alias inSelectionMode: select.inSelectionMode

    /// Quit selection mode, and unselect all photos
    function leaveSelectionMode() {
        selection.unselectAll()
        selection.inSelectionMode = false
    }

    selection: SelectionState {
        id: select
    }

    model: EventCollectionModel {
    }

    delegate: OrganicMediaList {
        id: photosList
        objectName: "organicEventItem" + index

        width: organicEventView.width
        event: model.event
        selection: organicEventView.selection

        onPressed: {
            var rect = GalleryUtility.translateRect(thumbnailRect, photosList,
                                                    organicEventView);
            organicEventView.mediaSourcePressed(mediaSource, rect);
        }
    }

    Component {
        id: deleteDialog
        DeleteDialog {
            title: i18n.tr("Delete %1 photo", "Delete %1 photos", organicEventView.selection.selectedCount).arg(organicEventView.selection.selectedCount)

            onDeleteClicked: {
                organicEventView.selection.model.destroySelectedMedia();
                organicEventView.leaveSelectionMode();
            }
        }
    }

    property Item overviewTools: PhotosToolbarActions {
        selection: organicEventView.selection
    }

    property int __pickerContentHeight: height - units.gu(20)
    property PopupAlbumPicker __albumPicker
    Connections {
        target: __albumPicker
        onAlbumPicked: {
            album.addSelectedMediaSources(selection.model.selectedMedias);
            organicEventView.leaveSelectionMode();
        }
    }

    property Item selectionTools: SelectionToolbarAction {
        selection: organicEventView.selection

        onCancelClicked: {
            organicEventView.leaveSelectionMode();
        }
        onAddClicked: {
            __albumPicker = PopupUtils.open(Qt.resolvedUrl("Components/PopupAlbumPicker.qml"),
                                            null,
                                            {contentHeight: organicEventView.__pickerContentHeight});
        }
        onDeleteClicked: {
            PopupUtils.open(deleteDialog, null);
        }

        onShareClicked: {
            overview.pushPage(sharePicker);
            sharePicker.visible = true;
        }
    }

    property bool selectionMode: selection.inSelectionMode
    tools: selectionMode ? selectionTools : overviewTools

    Component {
        id: contentItemComp
        ContentItem {}
    }

    Page {
        id: sharePicker
        visible: false

        ContentPeerPicker {
            objectName: "sharePickerEvents"
            anchors.fill: parent

            contentType: organicEventView.selection.mediaType === MediaSource.Video ? ContentType.Videos : ContentType.Pictures
            handler: ContentHandler.Share

            onPeerSelected: {
                overview.popPage();
                sharePicker.visible = false;

                var curTransfer = peer.request();
                if (curTransfer.state === ContentTransfer.InProgress)
                {
                    var medias = organicEventView.selection.model.selectedMediasQML;
                    curTransfer.items = medias.filter(function(data) {
                        return data.hasOwnProperty('type'); // filter out event headers
                    }).map(function(data) {
                        return contentItemComp.createObject(parent, {"url": data.path});
                    });
                    curTransfer.state = ContentTransfer.Charged;
                }
            }
            onCancelPressed: {
                overview.popPage();
                sharePicker.visible = false;
            }
        }
    }
}

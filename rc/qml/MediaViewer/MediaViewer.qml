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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Content 0.1
import "../Components"
import "../Utility"
import "../../js/Gallery.js" as Gallery

/*!
*/
Item {
    id: viewerWrapper

    /*!
    */
    property alias media: galleryPhotoViewer.media
    /*!
    */
    property alias model: galleryPhotoViewer.model
    /*!
    */
    property alias index: galleryPhotoViewer.index
    /*!
    */
    property alias currentIndexForHighlight:
        galleryPhotoViewer.currentIndexForHighlight

    // Set this when entering from an album.
    property variant album

    // Read-only
    // Set to true when an image is loaded and displayed.
    //
    // NOTE: The empty-model check does perform a useful function here and should NOT be
    // removed; for whatever reason, it's possible to get here and have what would have
    // been the current item be deleted, but not be null, and what it actually points to
    // is no longer valid and will result in an immediate segfault if dereferenced.
    //
    // Since there is no current item if there are no more photo objects left in the model,
    // the check catches this before we can inadvertently follow a stale pointer.
    property bool isReady: model != null && model.count > 0 && galleryPhotoViewer.currentItem

    // tooolbar actions for the full view
    property variant actions: (media && !sharePicker.visible) ? d.mediaActions : []

    property variant backAction: d.backAction

    /*!
    */
    signal closeRequested()
    /*!
    */
    signal editRequested(variant photo)

    /*!
    */
    function setCurrentIndex(index) {
        galleryPhotoViewer.setCurrentIndex(index);
    }

    /*!
    */
    function setCurrentPhoto(photo) {
        galleryPhotoViewer.setCurrentPhoto(photo);
    }

    /*!
    */
    function goBack() {
        galleryPhotoViewer.goBack();
    }

    /*!
    */
    function goForward() {
        galleryPhotoViewer.goForward();
    }

    function closeMediaViewer()
    {
        galleryPhotoViewer.currentItem.reset();
        closeRequested();
    }

    Rectangle{
        color: "black"
        anchors.fill: parent
    }

    MediaListView {
        id: galleryPhotoViewer
        objectName: "mediaListView"

        // When the user clicks the back button.
        signal closeRequested()
        signal editRequested(variant photo) // The user wants to edit this photo.

        // NOTE: These properties should be treated as read-only, as setting them
        // individually can lead to bogus results.  Use setCurrentPhoto() or
        // setCurrentIndex() to initialize the view.
        property variant media: null

        function setCurrentPhoto(photo) {
            setCurrentIndex(model.indexOf(photo));
        }

        function goBack() {
            galleryPhotoViewer.currentItem.reset();
            pageBack();
        }

        function goForward() {
            galleryPhotoViewer.currentItem.reset();
            pageForward();
        }

        anchors.fill: parent

        onCurrentIndexChanged: {
            if (model)
                media = model.getAt(currentIndex);
        }

        delegate: SingleMediaViewer {
            id: media
            objectName: "openedMedia" + index
            mediaSource: model.mediaSource

            width: galleryPhotoViewer.width
            height: galleryPhotoViewer.height

            // Needed as ListView.isCurrentItem can't be used directly in a change handler
            property bool isActive: ListView.isCurrentItem
            onIsActiveChanged: if (!isActive) reset();

            opacity: {
                if (!galleryPhotoViewer.moving || galleryPhotoViewer.contentX < 0
                        || index != galleryPhotoViewer.currentIndexForHighlight)
                    return 1.0;

                return 1.0 - Math.abs((galleryPhotoViewer.contentX - x) / width);
            }

            onClicked: overview.toggleHeaderVisibility()
        }

        // Don't allow flicking while the chrome is actively displaying a popup
        // menu, or the image is zoomed. When images are zoomed,
        // mouse drags should pan, not flick.
        interactive: currentItem != null &&
                     !currentItem.userInteracting  // FIXME: disable when editing ??

        Component {
            id: contentItemComp
            ContentItem {}
        }

        Rectangle {
            id: sharePicker
            anchors.fill: parent
            visible: false

            onVisibleChanged: overview.setHeaderVisibility(!visible, false)

            ContentPeerPicker {
                objectName: "sharePicker"
                anchors.fill: parent
                visible: parent.visible
                contentType: galleryPhotoViewer.media.type === MediaSource.Video ? ContentType.Videos : ContentType.Pictures
                handler: ContentHandler.Share

                onPeerSelected: {
                    parent.visible = false;
                    var curTransfer = peer.request();
                    if (curTransfer.state === ContentTransfer.InProgress)
                    {
                        curTransfer.items = [ contentItemComp.createObject(parent, {"url": viewerWrapper.media.path}) ];
                        curTransfer.state = ContentTransfer.Charged;
                    }
                }
                onCancelPressed: parent.visible = false;
            }
        }

        Component {
            id: deleteDialog
            Dialog {
                id: dialogue
                objectName: "deletePhotoDialog"
                title: (galleryPhotoViewer.media.type === MediaSource.Photo) ? i18n.tr("Delete a photo") : i18n.tr("Delete a video")

                function finishRemove() {
                    if (!album === undefined)
                        return;
                    if (model.count === 0)
                        galleryPhotoViewer.closeRequested();
                }

                Button {
                    objectName: "deletePhotoDialogYes"
                    text: i18n.tr("Yes")
                    color: Gallery.HIGHLIGHT_BUTTON_COLOR
                    onClicked: {
                        PopupUtils.close(dialogue)
                        viewerWrapper.model.destroyMedia(galleryPhotoViewer.media, true);
                        galleryPhotoViewer.currentIndexChanged();
                        dialogue.finishRemove();
                    }
                }
                Button {
                    objectName: "deletePhotoDialogNo"
                    text: i18n.tr("No")
                    onClicked: PopupUtils.close(dialogue)
                }
            }
        }

        Component {
            id: removeFromAlbumDialog
            Dialog {
                id: dialogue
                objectName: "removePhotoFromAlbumDialog"
                title: (galleryPhotoViewer.media.type === MediaSource.Photo) ? i18n.tr("Remove a photo from album") : i18n.tr("Remove a video from album")

                function finishRemove() {
                    if (model.count === 0)
                        galleryPhotoViewer.closeRequested();
                }

                Button {
                    objectName: "removeFromAlbumButton"
                    text: i18n.tr("Remove from Album")
                    color: Gallery.HIGHLIGHT_BUTTON_COLOR
                    onClicked: {
                        PopupUtils.close(dialogue)
                        viewerWrapper.model.removeMediaFromAlbum(album, galleryPhotoViewer.media);
                        dialogue.finishRemove();
                    }
                }

                Button {
                    objectName: "removeFromAlbumAndDeleteButton"
                    text: i18n.tr("Remove from Album and Delete")
                    onClicked: {
                        PopupUtils.close(dialogue)
                        viewerWrapper.model.destroyMedia(galleryPhotoViewer.media, true);
                        dialogue.finishRemove();
                    }
                }

                Button {
                    objectName: "removeFromAlbumCancelButton"
                    text: i18n.tr("Cancel")
                    onClicked: PopupUtils.close(dialogue)
                }
            }

        }

        onCloseRequested: viewerWrapper.closeRequested()
        onEditRequested: viewerWrapper.editRequested(media)
    }

    property int __pickerContentHeight: galleryPhotoViewer.height - units.gu(10)
    property PopupAlbumPicker __albumPicker
    Connections {
        target: __albumPicker
        onAlbumPicked: {
            album.addMediaSource(galleryPhotoViewer.media);
        }
        onNewAlbumPicked: {
            viewerWrapper.closeRequested();
        }
    }  

    ActivityIndicator {
        id: busySpinner
        objectName: "busySpinner"
        anchors.centerIn: parent
        visible: media ? media.busy : false
        running: visible
    }

    Item {
        id: d

        property list<Action> mediaActions: [
            Action {
                objectName: "editButton"
                text: i18n.tr("Edit")
                iconSource: "../../img/edit.png"
                enabled: galleryPhotoViewer.media.type === MediaSource.Photo && galleryPhotoViewer.media.canBeEdited
                onTriggered: {
                    var path = galleryPhotoViewer.media.path.toString();
                    path = path.replace("file://", "")
                    var editor;
                    try {
                        Qt.createQmlObject('import QtQuick 2.0; import Ubuntu.Components.Extras 0.2; Item {}', viewerWrapper);
                        console.log("Loading PhotoEditor Components from Extras");
                        editor = overview.pushPage(Qt.resolvedUrl("ExtrasPhotoEditorPage.qml"), { photo: path });
                    } catch (e) {
                        console.log("Loading PhotoEditor Components from Gallery code");
                        editor = overview.pushPage(Qt.resolvedUrl("GalleryPhotoEditorPage.qml"), { photo: path });
                    }
                    editor.done.connect(function(photoWasModified) {
                        if (photoWasModified) galleryPhotoViewer.media.dataChanged();
                        overview.popPage();
                    });
                }
            },
            Action {
                objectName: "addButton"
                text: i18n.tr("Add to album")
                iconName: "add"
                onTriggered: {
                    __albumPicker = PopupUtils.open(Qt.resolvedUrl("../Components/PopupAlbumPicker.qml"),
                                                    null,
                                                    {contentHeight: viewerWrapper.__pickerContentHeight});
                }
            },
            Action {
                objectName: "deleteButton"
                text: i18n.tr("Delete")
                iconName: "delete"
                onTriggered: {
                    if (album)
                        PopupUtils.open(removeFromAlbumDialog, null);
                    else
                        PopupUtils.open(deleteDialog, null);
                }
            },
            Action {
                objectName: "shareButton"
                text: i18n.tr("Share")
                iconName: "share"
                visible: !APP.desktopMode
                onTriggered: sharePicker.visible = true;
            }
        ]
 
        property Action backAction: Action {
            iconName: "back"
            onTriggered: {
                galleryPhotoViewer.currentItem.reset();
                closeRequested();
            }
        }
    }

    Rectangle{
        id: headerBackground
        width: parent.width
        height: header.height
        visible: header.visible
    }
}

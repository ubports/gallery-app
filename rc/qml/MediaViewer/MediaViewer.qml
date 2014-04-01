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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Extras 0.1
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
    property bool isReady: model != null && model.count > 0 &&
                           (galleryPhotoViewer.currentItem ? galleryPhotoViewer.currentItem.isLoaded : false)

    // tooolbar actions for the full view
    property Item tools: media ? (media.type === MediaSource.Photo ?
                                            d.photoToolbar : d.videoToolbar)
                                         : null

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

    // If the media item is a video, start playing it
    function playVideo()
    {
        if (!galleryPhotoViewer.currentItem)
            return;
        if (galleryPhotoViewer.media.type !== MediaSource.Video)
            return;

        if (galleryPhotoViewer.currentItem.isPlayingVideo)
            return;

        galleryPhotoViewer.currentItem.togglePlayPause();
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

        delegate: Item {
            /// Is true while the media content is loaded
            property bool isLoaded: delegateView.item.isLoaded
            /// Is true when the view is in a state, where the user possibly
            /// interacts with the media (and not swipe to another media)
            property bool userInteracting: delegateView.item.state === "zoomed"
            /// Needed as ListView.isCurrentItem can't be used directly
            property bool isActive: ListView.isCurrentItem
            /// True if a video is currently played
            property bool isPlayingVideo: galleryPhotoViewer.currentItem ?
                                              galleryPhotoViewer.currentItem.state === "playing"
                                            : false

            // set the view to it's original state
            function reset() {
                delegateView.item.reset();
            }
            /// Toggles between play and pause - only usful when a video is shown
            function togglePlayPause() {
                if (model.mediaSource.type === MediaSource.Video)
                    delegateView.item.togglePlayPause();
            }

            onIsActiveChanged: {
                if (!isActive)
                    reset()
            }

            width: galleryPhotoViewer.width
            height: galleryPhotoViewer.height
            state: delegateView.item.state

            opacity: {
                if (!galleryPhotoViewer.moving || galleryPhotoViewer.contentX < 0
                        || index != galleryPhotoViewer.currentIndexForHighlight)
                    return 1.0;

                return 1.0 - Math.abs((galleryPhotoViewer.contentX - x) / width);
            }

            Component {
                id: component_delegatePhotoView
                PhotoViewerDelegate {
                    useInteractivePreview: false
                    mediaSource: model.mediaSource
                }
            }
            Component {
                id: component_delegateVideoView
                VideoViewerDelegate {
                    mediaSource: model.mediaSource
                }
            }
            Loader {
                id: delegateView
                anchors.fill: parent
                sourceComponent: model.mediaSource.type === MediaSource.Photo ?
                                     component_delegatePhotoView : component_delegateVideoView
            }

        }

        // Don't allow flicking while the chrome is actively displaying a popup
        // menu, or the image is zoomed, or we're cropping. When images are zoomed,
        // mouse drags should pan, not flick. Also don't flick during parameterized
        // HUD action to prevent photo from changing during the action
        interactive: currentItem != null &&
                     !currentItem.userInteracting &&
                     cropper.state == "hidden" &&
                     !editHUD.actionActive

        Component {
            id: shareConfigurePopover

            ActionSelectionPopover {
                objectName: "shareConfigurePopover"
                visible: false
                actions: ActionList {
                    Action {
                        text: i18n.tr("Configure Facebook for photo sharing...")
                        onTriggered: {
                            Qt.openUrlExternally("settings:///system/online-accounts");
                        }
                    }
                }
            }
        }

        FacebookAccount {
            id: facebook

            property ActionSelectionPopover _lastPopover: null

            function configureOrStartSharing(button) {
                if (_lastPopover) {
                    PopupUtils.close(_lastPopover);
                    _lastPopover = null;
                } else {
                    if (facebook.id == -1) {
                        _lastPopover = PopupUtils.open(shareConfigurePopover, button);
                    } else {
                        sharePanel.fileToShare = viewerWrapper.media.path;
                        sharePanel.userAccountId = facebook.id;
                        sharePanel.visible = true;
                        viewerWrapper.tools.opened = false;
                         _lastPopover = null;
                    }
                }
            }
        }

        Component {
            id: editPopoverComponent
            EditPopover {
                id: editPopover
                objectName: "editPopover"
                visible: false
                photo: galleryPhotoViewer.media
                cropper: viewerWrapper.cropper

                onButtonPressed: {
                    viewerWrapper.tools.opened = false;
                }
            }
        }

        Component {
            id: deleteDialog
            Dialog {
                id: dialogue
                objectName: "deletePhotoDialog"
                title: i18n.tr("Delete a photo")

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

    property alias cropper: cropper
    CropInteractor {
        id: cropper

        property var targetPhoto

        function show(photo) {
            targetPhoto = photo;

            fadeOutPhotoAnimation.running = true;
        }

        function hide() {
            state = "hidden";
            galleryPhotoViewer.opacity = 0.0;
            galleryPhotoViewer.visible = true;
            fadeInPhotoAnimation.running = true;
        }

        state: "hidden"
        states: [
            State { name: "shown";
                PropertyChanges { target: cropper; visible: true; }
                PropertyChanges { target: cropper; opacity: 1.0; }
            },
            State { name: "hidden";
                PropertyChanges { target: cropper; visible: false; }
                PropertyChanges { target: cropper; opacity: 0.0; }
            }
        ]

        Behavior on opacity {
            NumberAnimation { duration: Gallery.FAST_DURATION }
        }

        anchors.fill: parent

        MouseArea {
            id: blocker

            visible: cropper.state == "shown"
            anchors.fill: parent

            onClicked: { }
        }

        onCanceled: {
            photo.cancelCropping();
            hide();
            targetPhoto = null;
        }

        onCropped: {
            var qtRect = Qt.rect(rect.x, rect.y, rect.width, rect.height);
            photo.crop(qtRect);
            hide();
            targetPhoto = null;
        }

        onOpacityChanged: {
            if (opacity == 1.0)
                galleryPhotoViewer.visible = false
        }

        NumberAnimation {
            id: fadeOutPhotoAnimation

            from: 1.0
            to: 0.0
            target: galleryPhotoViewer
            property: "opacity"
            duration: Gallery.FAST_DURATION
            easing.type: Easing.InOutQuad

            onRunningChanged: {
                if (running == false) {
                    var ratio_crop_rect = cropper.targetPhoto.prepareForCropping();
                    cropper.enter(cropper.targetPhoto, ratio_crop_rect);
                    cropper.state = "shown";
                }
            }
        }

        NumberAnimation {
            id: fadeInPhotoAnimation

            from: 0.0
            to: 1.0
            target: galleryPhotoViewer
            property: "opacity"
            duration: Gallery.FAST_DURATION
            easing.type: Easing.InOutQuad
        }
    }

    EditPreview {
        id: editPreview
        objectName: "editPreview"
        anchors.fill: parent
        source: galleryPhotoViewer.media ? galleryPhotoViewer.media.galleryPreviewPath : ""

        visible: editHUD.actionActive

        exposure: editHUD.exposureValue

        brightness: editHUD.brightness
        contrast: editHUD.contrast
        saturation: editHUD.saturation
        hue: editHUD.hue
    }

    ActivityIndicator {
        id: busySpinner
        objectName: "busySpinner"
        anchors.centerIn: parent
        visible: media ? media.busy : false
        running: visible
    }

    EditingHUD {
        id: editHUD
        photo: galleryPhotoViewer.media
        onExposureActivated: editPreview.useExposure()
        onColorBalanceActivated: editPreview.useColorBalance()
    }

    Item {
        id: d

        property Item photoToolbar: ToolbarItems {
            ToolbarButton {
                id: photoEditButton
                objectName: "editButton"
                action: Action {
                    text: i18n.tr("Edit")
                    iconSource: "../../img/edit.png"
                    onTriggered: {
                        PopupUtils.open(editPopoverComponent, photoEditButton);
                    }
                }
            }
            ToolbarButton {
                id: photoAddButton
                objectName: "addButton"
                action: Action {
                    text: i18n.tr("Add photo to album")
                    iconSource: "../../img/add.png"
                    onTriggered: {
                        __albumPicker = PopupUtils.open(Qt.resolvedUrl("../Components/PopupAlbumPicker.qml"),
                                                        photoAddButton,
                                                        {contentHeight: viewerWrapper.__pickerContentHeight});
                    }
                }
                text: i18n.tr("Add")
            }
            ToolbarButton {
                objectName: "deleteButton"
                action: Action {
                    text: i18n.tr("Delete")
                    iconSource: "../../img/delete.png"
                    onTriggered: {
                        PopupUtils.open(deleteDialog, null);
                    }
                }
                text: i18n.tr("Delete")
            }
            ToolbarButton {
                id: photoShareButton
                objectName: "shareButton"
                action: Action {
                    text: i18n.tr("Share photo")
                    iconSource: "../../img/share.png"
                    onTriggered: {
                        facebook.configureOrStartSharing(photoShareButton);
                    }
                }
                text: i18n.tr("Share")
            }

            back: ToolbarButton {
                objectName: "backButton"
                text: i18n.tr("Back")
                iconSource: "../../img/back.png"
                onTriggered: {
                    galleryPhotoViewer.currentItem.reset();
                    closeRequested();
                }
            }
        }

        property Item videoToolbar: ToolbarItems {
            ToolbarButton {
                text: galleryPhotoViewer.currentItem ?
                          (galleryPhotoViewer.currentItem.isPlayingVideo ?
                               i18n.tr("Pause") : i18n.tr("Play"))
                        : ""
                iconSource: galleryPhotoViewer.currentItem ?
                                (galleryPhotoViewer.currentItem.isPlayingVideo ?
                                     "../../img/icon_pause.png" : "../../img/icon_play.png")
                              : ""
                onTriggered: {
                    galleryPhotoViewer.currentItem.togglePlayPause();
                }
            }
            ToolbarButton {
                id: videoAddButton
                objectName: "addButton"
                text: i18n.tr("Add")
                iconSource: "../../img/add.png"
                onTriggered: {
                    __albumPicker = PopupUtils.open(Qt.resolvedUrl("../Components/PopupAlbumPicker.qml"),
                                                    videoAddButton,
                                                    {contentHeight: viewerWrapper.__pickerContentHeight});
                }
            }
            ToolbarButton {
                objectName: "deleteButton"
                text: i18n.tr("Delete")
                iconSource: "../../img/delete.png"
                onTriggered: {
                    PopupUtils.open(deleteDialog, null);
                }
            }
            ToolbarButton {
                id: videoShareButton
                objectName: "shareButton"
                text: i18n.tr("Share")
                iconSource: "../../img/share.png"
                onTriggered: {
                    facebook.configureOrStartSharing(videoShareButton);
                }
            }

            back: ToolbarButton {
                objectName: "backButton"
                text: i18n.tr("Back")
                iconSource: "../../img/back.png"
                onTriggered: {
                    galleryPhotoViewer.currentItem.reset();
                    closeRequested();
                }
            }
        }
    }

    Loader {
        id: sharePanel
        anchors.fill: parent
        visible: false
        sourceComponent: (fileToShare !== "" && userAccountId !== "") ? component_sharePanel : null

        property string fileToShare
        property string userAccountId

        Component {
            id: component_sharePanel
            Share {
                fileToShare: sharePanel.fileToShare
                userAccountId: sharePanel.userAccountId
                onUploadCompleted: sharePanel.visible = false;
                onCanceled: sharePanel.visible = false;
            }
        }
    }

}

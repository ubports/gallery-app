# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils

class PhotoViewer(GalleryUtils):

    def __init__(self, app):
        self.app = app

    def get_first_image_in_event_view(self):
        """Returns the first photo of the gallery."""
        event = self.get_first_event()
        list_view = event.get_children_by_type("QQuickListView")[0]
        item = list_view.get_children_by_type("QQuickItem")[0]
        first_photo_delegate = item.get_children_by_type("QQuickItem", objectName="eventPhoto")[0]
        first_photo = first_photo_delegate.get_children_by_type("UbuntuShape")[0]
        return first_photo

    def get_first_event(self):
        """Returns the first event in the event view"""
        return self.app.select_single("OrganicMediaList", objectName="organicEventItem0")

    def get_viewer_chrome_toolbar_edit_button(self):
        """Return the edit button of the toolbar when photo viewer is shown"""
        return self.get_toolbar_button(0)

    def get_viewer_chrome_album_button(self):
        """Returns the photo viewer album button."""
        return self.app.select_single("ChromeButton", objectName='disabled', visible=True)

    def get_delete_icon(self):
        """Return the delete button of the toolbar when photo viewer is shown"""
        return self.get_toolbar_button(2)

    def get_viewer_chrome_share_button(self):
        """Return the share button of the toolbar when photo viewer is shown"""
        return self.get_toolbar_button(3)

    def get_delete_dialog(self):
        """Returns the photo viewer delete dialog."""
        return self.select_single_retry("Dialog", objectName="deletePhotoDialog")

    def get_popup_album_picker(self):
        """Returns the photo viewer album pickers."""
        return self.app.select_single("PopupAlbumPicker", objectName="popupAlbumPicker")

    def get_share_dialog(self):
        """Returns the photo viewer share dialog."""
        return self.app.select_single("SharePopover", objectName="sharePopover")

    def get_photo_edit_dialog(self):
        """Returns the photo edit dialog."""
        return self.app.select_single("EditPopover", objectName="editPopover")

    def get_photo_component(self):
        return self.app.select_many("ZoomablePhotoComponent", ownerName="photoViewerDelegate")[0]

    def get_crop_menu_item(self):
        """Returns the crop item of the edit dialog."""
        return self.app.select_single("Standard", objectName="cropListItem")

    def get_rotate_menu_item(self):
        """Returns the rotate item of the edit dialog."""
        return self.app.select_single("Standard", objectName="rotateListItem")

    def get_undo_menu_item(self):
        """Returns the undo item of the edit dialog."""
        return self.app.select_single("Standard", objectName="undoListItem")

    def get_redo_menu_item(self):
        """Returns the redo item of the edit dialog."""
        return self.app.select_single("Standard", objectName="redoListItem")

    def get_revert_menu_item(self):
        """Returns the revert to original menu item in the edit dialog."""
        return self.app.select_single("Standard", objectName="revertListItem")

    def get_delete_popover_delete_item(self):
        """Returns the delete button of the delete popover."""
        return self.app.select_single("Button", objectName="deletePhotoDialogYes", visible=True)

    def get_delete_popover_cancel_item(self):
        """Returns the cancel button of the delete popover."""
        return self.app.select_single("Button", objectName="deletePhotoDialogNo", visible=True)

    def get_opened_photo(self):
        """Returns the first opened photo."""
        return self.app.select_single("ZoomablePhotoComponent", objectName="openedPhoto0")

    def get_crop_interactor(self):
        """Returns the crop interactor."""
        return self.app.select_single("CropInteractor", objectName="cropInteractor")

    def get_crop_overlay(self):
        """Returns the crop overlay."""
        return self.app.select_single("CropOverlay", objectName="cropOverlay")

    def get_top_left_crop_corner(self):
        """Returns the top left corner of the crop overlay for dragging."""
        return self.app.select_single("CropCorner", objectName="topLeftCropCorner")

    def get_crop_overlays_crop_icon(self):
        """Returns the crop icon of the crop overlay."""
        return self.app.select_single("Button", objectName="centerCropIcon", visible=True)

    def get_edit_preview(self):
        """Returns the edit preview."""
        return self.app.select_single("EditPreview", objectName="editPreview")

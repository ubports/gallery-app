# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import ubuntuuitoolkit.emulators

from gallery_utils import GalleryUtils


class PhotoViewer(GalleryUtils):

    def __init__(self, app):
        self.app = app
        self.pointing_device = ubuntuuitoolkit.emulators.get_pointing_device()

    def get_delete_dialog(self):
        """Returns the photo viewer delete dialog."""
        return self.app.wait_select_single("Dialog",
                                           objectName="deletePhotoDialog")

    def delete_dialog_shown(self):
        dialog = self.app.select_many("Dialog",
                                      objectName="deletePhotoDialog")
        return len(dialog) >= 1

    def get_popup_album_picker(self):
        """Returns the photo viewer album pickers."""
        return self.app.wait_select_single("PopupAlbumPicker",
                                           objectName="popupAlbumPicker")

    def get_share_dialog(self):
        """Returns the photo viewer share dialog."""
        return self.app.wait_select_single("SharePopover",
                                           objectName="sharePopover")

    def get_photo_edit_dialog(self):
        """Returns the photo edit dialog."""
        return self.app.wait_select_single("EditPopover",
                                           objectName="editPopover")

    def get_photo_component(self):
        # Was using a list index (lp:1247711). Still needs fixing, I'm not
        # convinced this is a suitable way to select the correct item.
        return self.app.wait_select_single(
            "ZoomablePhotoComponent",
            ownerName="photoViewerDelegate",
            objectName="openedPhoto0"
        )

    def get_photos_list(self):
        return self.app.wait_select_single("MediaListView")

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

    def get_auto_enhance_menu_item(self):
        """Returns the 'auto enhance' menu item in the edit dialog."""
        return self.app.select_single("Standard", objectName='enhanceListItem')

    def get_delete_popover_delete_item(self):
        """Returns the delete button of the delete popover."""
        return self.app.select_single("Button",
                                      objectName="deletePhotoDialogYes",
                                      visible=True)

    def get_delete_popover_cancel_item(self):
        """Returns the cancel button of the delete popover."""
        return self.app.select_single("Button",
                                      objectName="deletePhotoDialogNo",
                                      visible=True)

    def get_opened_photo(self):
        """Returns the first opened photo."""
        return self.app.wait_select_single("ZoomablePhotoComponent",
                                           objectName="openedPhoto0")

    def get_crop_interactor(self):
        """Returns the crop interactor."""
        return self.app.wait_select_single("CropInteractor",
                                           objectName="cropInteractor")

    def get_crop_overlay(self):
        """Returns the crop overlay."""
        return self.app.wait_select_single("CropOverlay",
                                           objectName="cropOverlay")

    def get_top_left_crop_corner(self):
        """Returns the top left corner of the crop overlay for dragging."""
        return self.app.wait_select_single("CropCorner",
                                           objectName="topLeftCropCorner")

    def get_crop_overlays_crop_icon(self):
        """Returns the crop icon of the crop overlay."""
        return self.app.wait_select_single("Button",
                                           objectName="centerCropIcon",
                                           visible=True)

    def get_edit_preview(self):
        """Returns the edit preview."""
        return self.app.wait_select_single("EditPreview",
                                           objectName="editPreview")

    def _click_item(self, item):
        self.pointing_device.click_object(item)

    def click_rotate_item(self):
        rotate_item = self.get_rotate_menu_item()
        self._click_item(rotate_item)

    def click_crop_item(self):
        crop_item = self.get_crop_menu_item()
        self._click_item(crop_item)

    def click_undo_item(self):
        undo_item = self.get_undo_menu_item()
        self._click_item(undo_item)

    def click_redo_item(self):
        redo_item = self.get_redo_menu_item()
        self._click_item(redo_item)

    def click_revert_item(self):
        revert_item = self.get_revert_menu_item()
        self._click_item(revert_item)

    def click_enhance_item(self):
        enhance_item = self.get_auto_enhance_menu_item()
        self._click_item(enhance_item)

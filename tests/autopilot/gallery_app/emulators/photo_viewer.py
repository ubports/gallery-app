# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import logging

import autopilot.logging
import ubuntuuitoolkit

from gallery_app.emulators import main_screen
from gallery_app.emulators.gallery_utils import(
    GalleryAppException,
    GalleryUtils
)


logger = logging.getLogger(__name__)


class PopupPhotoViewer(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    @autopilot.logging.log_action(logger.info)
    def delete_current_photo(self, confirm=True):
        header = self.get_root_instance().select_single(
            main_screen.MainScreen).get_header()
        header.click_action_button("deleteButton")
        if confirm:
            self.confirm_delete_photo()
        else:
            self.cancel_delete_photo()

    @autopilot.logging.log_action(logger.debug)
    def confirm_delete_photo(self):
        self._click_delete_dialog_button("Yes")

    def _click_delete_dialog_button(self, name):
        delete_dialog = self._get_delete_dialog()
        button = delete_dialog.wait_select_single(
            "Button", objectName="deletePhotoDialog" + name, visible=True)
        self.pointing_device.click_object(button)
        delete_dialog.wait_until_destroyed()

    def _get_delete_dialog(self):
        delete_dialog = self.get_root_instance().wait_select_single(
            objectName="deletePhotoDialog")
        delete_dialog.visible.wait_for(True)
        delete_dialog.opacity.wait_for(1)
        return delete_dialog

    @autopilot.logging.log_action(logger.debug)
    def cancel_delete_photo(self):
        self._click_delete_dialog_button('No')


class PhotoViewer(GalleryUtils):

    def __init__(self, app):
        super(PhotoViewer, self).__init__(self)
        self.app = app

    def get_popup_album_picker(self):
        """Returns the photo viewer album pickers."""
        return self.app.wait_select_single("PopupAlbumPicker",
                                           objectName="popupAlbumPicker")

    def get_content_peer_picker_cancel_button(self):
        """Returns the ContentPeerPicker cancel button."""
        return self.get_share_peer_picker().wait_select_single(
            "Button",
            objectName="contentPeerPickerCancelButton",
            visible=True)

    def get_share_peer_picker(self):
        """Returns the photo viewer share picker."""
        return self.app.wait_select_single("ContentPeerPicker10",
                                           objectName="sharePicker",
                                           visible=True)

    def get_photo_editor(self):
        """Returns the photo edit dialog."""
        return self.app.wait_select_single("PhotoEditor")

    def get_revert_to_original_dialog(self):
        """Returns the revert to original dialog."""
        return self.app.wait_select_single("Dialog",
                                           objectName="revertPromptDialog")

    def get_cancel_revert_to_original_button(self):
        """Returns the revert to original cancel button."""
        return self.get_revert_to_original_dialog().wait_select_single(
            "Button",
            objectName="cancelRevertButton",
            visible=True)

    def get_confirm_revert_to_original_button(self):
        """Returns the revert to original confirm button."""
        return self.get_revert_to_original_dialog().wait_select_single(
            "Button",
            objectName="confirmRevertButton",
            visible=True)

    def get_photo_component(self):
        # Was using a list index (lp:1247711). Still needs fixing, I'm not
        # convinced this is a suitable way to select the correct item.
        return self.app.wait_select_single(
            "SingleMediaViewer",
            objectName="openedMedia0"
        )

    def get_photos_list(self):
        return self.app.wait_select_single("MediaListView")

    def get_editor_actions_bar(self):
        """Returns the actions bar for the editor."""
        return self.app.select_single("ActionsBar",
                                      objectName="editorActionsBar")

    def get_editor_action_button_by_text(self, button_text):
        """Returns the action button from the editor by text."""
        actions_bar = self.get_editor_actions_bar()
        buttons = actions_bar.select_many('AbstractButton')
        for button in buttons:
            if str(button.text) == button_text:
                return button
        raise GalleryAppException(
            'Editor action button {} could not be found'.format(button_text))

    def get_crop_action_button(self):
        """Returns the crop item of the edit dialog."""
        return self.get_editor_action_button_by_text("Crop")

    def get_rotate_action_button(self):
        """Returns the rotate item of the edit dialog."""
        return self.get_editor_action_button_by_text("Rotate")

    def get_undo_menu_item(self):
        """Returns the undo item of the edit dialog."""
        return self.app.select_single("Standard", objectName="undoListItem")

    def get_redo_menu_item(self):
        """Returns the redo item of the edit dialog."""
        return self.app.select_single("Standard", objectName="redoListItem")

    def get_revert_action_button(self):
        """Returns the revert to original menu item in the edit dialog."""
        return self.get_editor_action_button_by_text("Revert to Original")

    def get_auto_enhance_menu_item(self):
        """Returns the 'auto enhance' menu item in the edit dialog."""
        return self.app.select_single("Standard", objectName='enhanceListItem')

    def get_delete_popover_cancel_item(self):
        """Returns the cancel button of the delete popover."""
        return self.app.wait_select_single("Button",
                                           objectName="deletePhotoDialogNo",
                                           visible=True)

    def get_opened_photo(self):
        """Returns the first opened photo."""
        return self.app.wait_select_single("SingleMediaViewer",
                                           objectName="openedMedia0")

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

    def click_rotate_button(self):
        rotate_item = self.get_rotate_action_button()
        self._click_item(rotate_item)

    def click_crop_button(self):
        crop_item = self.get_crop_action_button()
        self._click_item(crop_item)

    def click_undo_item(self):
        undo_item = self.get_undo_menu_item()
        self._click_item(undo_item)

    def click_redo_item(self):
        redo_item = self.get_redo_menu_item()
        self._click_item(redo_item)

    def click_revert_button(self):
        revert_item = self.get_revert_action_button()
        self._click_item(revert_item)

    def click_cancel_revert_button(self):
        cancel_item = self.get_cancel_revert_to_original_button()
        self._click_item(cancel_item)

    def click_confirm_revert_button(self):
        confirm_item = self.get_confirm_revert_to_original_button()
        self._click_item(confirm_item)

    def click_enhance_item(self):
        enhance_item = self.get_auto_enhance_menu_item()
        self._click_item(enhance_item)

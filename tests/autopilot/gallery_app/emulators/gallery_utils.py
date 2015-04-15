# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
import ubuntuuitoolkit.emulators

from autopilot.testcase import AutopilotTestCase

from time import sleep


class GalleryAppException(Exception):
    pass


class GalleryUtils(AutopilotTestCase):
    """An emulator class that makes it easy to interact with
       general components of the gallery app."""

    retry_delay = 0.2

    def __init__(self, app):
        self.app = app
        self.pointing_device = ubuntuuitoolkit.emulators.get_pointing_device()

    def select_many_retry(self, object_type, **kwargs):
        """Returns the item that is searched for with app.select_many
        In case of no item was not found (not created yet) a second attempt is
        taken 1 second later"""
        items = self.app.select_many(object_type, **kwargs)
        tries = 10
        while len(items) < 1 and tries > 0:
            sleep(self.retry_delay)
            items = self.app.select_many(object_type, **kwargs)
            tries -= 1
        return items

    def get_qml_view(self):
        """Get the main QML view"""
        return self.app.select_single("QQuickView")

    def get_main_photo_viewer_loader(self):
        """Returns the loader item for the PhotoViewer."""
        return self.app.select_single("QQuickLoader",
                                      objectName="photoViewerLoader")

    def get_main_photo_viewer(self):
        """Returns the MediaListView."""
        return self.app.wait_select_single("MediaListView",
                                           objectName="mediaListView")

    def get_albums_viewer_loader(self):
        """Returns the loader item for the AlbumsOverview."""
        return self.app.select_single("QQuickLoader",
                                      objectName="albumsCheckerboardLoader")

    def get_delete_dialog(self):
        """Returns the delete dialog in the events view."""
        return self.app.wait_select_single("DeleteDialog",
                                           objectName="deleteDialog")

    def delete_dialog_shown(self):
        dialog = self.app.select_many(
            "DeleteDialog",
            objectName="deleteDialog")
        return len(dialog) >= 1

    def get_delete_dialog_delete_button(self):
        """Returns the delete button of the delete popover."""
        return self.app.wait_select_single("Button",
                                           objectName="deleteDialogYes",
                                           visible=True)

    def get_delete_dialog_cancel_button(self):
        """Returns the cancel button of the delete popover."""
        return self.app.wait_select_single("Button",
                                           objectName="deleteDialogNo",
                                           visible=True)

    def get_all_albums(self):
        """Returns all albums in the albums view"""
        albums = self.select_many_retry("CheckerboardDelegate",
                                        objectName="checkerboardDelegate")
        return albums

    def get_first_album(self):
        """Returns the first album in the albums view."""
        return self.get_album_at(0)

    def get_album_at(self, position):
        """Returns the albums at this position in the albums view"""
        albums = self.select_many_retry("CheckerboardDelegate",
                                        objectName="checkerboardDelegate")
        albums = sorted(
            albums, key=lambda album: (album.globalRect.y, album.globalRect.x))
        return albums[position]

    def get_edit_album_button(self):
        """Returns the edit album button in the album popover"""
        return self.app.select_single("Standard",
                                      objectName="editAlbumListItem")

    def get_cover_menu_item(self, name):
        """Returns the item of the cover menu with the name *name*

        :raises StateNotFoundError: if there is no item with the name *name*

        """
        return self.app.select_single(
            "Standard",
            objectName="albumCoverMenuItem_" + name
        )

    def _ensure_delete_dialog_visible(self):
        delete_dialog = self.get_delete_dialog()
        delete_dialog.opacity.wait_for(1)

    def _click_dialog_button(self, button):
        self._ensure_delete_dialog_visible()
        self.pointing_device.click_object(button)

    def click_delete_dialog_cancel_button(self):
        button = self.get_delete_dialog_cancel_button()
        self._click_dialog_button(button)

    def click_delete_dialog_delete_button(self):
        button = self.get_delete_dialog_delete_button()
        self._click_dialog_button(button)

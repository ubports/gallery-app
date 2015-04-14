# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators


class AlbumEditor(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """A class that makes it easy to interact with the album editor"""

    def close(self):
        cover_image = self.album_cover_image()
        # click left of the cover
        x, y, w, h = cover_image.globalRect
        self.pointing_device.move(x - 10, y + (h // 2))
        # workaround lp:1247698 (get rid of toolbar)
        self.pointing_device.click()
        self.pointing_device.click()
        self.ensure_fully_closed

    def ensure_fully_open(self):
        self.animationRunning.wait_for(False)
        self.active.wait_for(True)

    def ensure_fully_closed(self):
        self.active.wait_for(False)

    def album_editor(self):
        """Returns the album editor."""
        return self

    def album_title_entry_field(self):
        """Returns the album title input box."""
        editor = self.album_editor()
        return editor.wait_select_single("TextEditOnClick",
                                         objectName="albumTitleField")

    def click_title_field(self):
        self.pointing_device.click_object(self.album_title_entry_field())

    def album_subtitle_entry_field(self):
        """Returns the album subtitle input box."""
        editor = self.album_editor()
        return editor.wait_select_single("TextEditOnClick",
                                         objectName="albumSubtitleField")

    def click_subtitle_field(self):
        self.pointing_device.click_object(self.album_subtitle_entry_field())

    def album_cover_image(self):
        album_cover = self.wait_select_single("AlbumCover", visible=True)
        return album_cover.wait_select_single(
            "QQuickImage",
            objectName="albumCoverImage"
        )

    def add_photos(self):
        self.pointing_device.click_object(self._plus_icon())

    def _plus_icon(self):
        """Returns the plus icon to add photos"""
        return self.wait_select_single("QQuickImage",
                                       objectName="albumCoverAddPhotosImage",
                                       visible=True)

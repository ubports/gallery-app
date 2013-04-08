# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils

class AlbumEditor(GalleryUtils):
    """An emulator class that makes it easy to interact with the gallery app."""

    def __init__(self, app):
        self.app = app

    def get_edit_album_button(self):
        """Returns the edit album button in the album popover"""
        return self.app.select_single("Standard", objectName="editAlbumListItem")

    def get_animated_album_editor(self):
        """Returns the album editor."""
        return self.app.select_single("AlbumEditorAnimated", objectName="albumEditorAnimated")

    def get_album_editor(self):
        """Returns the album editor."""
        return self.app.select_single("AlbumEditor", objectName="mainAlbumEditor")

    def get_album_title_entry_field(self):
        """Returns the album title input box."""
        return self.app.select_many("TextEditOnClick", objectName="albumTitleField")[0]

    def get_album_subtitle_entry_field(self):
        """Returns the album subtitle input box."""
        return self.app.select_many("TextEditOnClick", objectName="albumSubtitleField")[0]

    def get_plus_icon(self):
        """Returns the plus icon to add photos"""
        return self.app.select_single("QQuickImage",
                                      objectName="albumCoverAddPhotosImage",
                                      visible=True)

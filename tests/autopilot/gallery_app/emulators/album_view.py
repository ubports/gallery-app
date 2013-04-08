# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils

class AlbumView(GalleryUtils):
    """An emulator class that makes it easy to interact with the gallery app."""

    def __init__(self, app):
        self.app = app

    def get_animated_album_view(self):
        """Returns the album view"""
        return self.app.select_single("AlbumViewerAnimated", objectName="albumViewerAnimated")

    def get_album_view(self):
        """Returns the album view"""
        return self.app.select_single("AlbumViewer", objectName="albumViewer")

    def get_first_photo(self):
        """Returns the first photo in a newly opened album"""
        return self.app.select_many("FramePortrait")[0]

    def get_album_photo_view(self):
        """Returns the photo view of the album viewer"""
        view = self.get_album_view()
        return view.get_children_by_type("PopupPhotoViewer")[0]

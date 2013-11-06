# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils


class AlbumView(GalleryUtils):
    """An emulator class that makes it easy to interact with the gallery app"""

    def __init__(self, app):
        self.app = app

    def get_animated_album_view(self):
        """Returns the album view"""
        return self.app.wait_select_single("AlbumViewerAnimated",
                                           objectName="albumViewerAnimated")

    def get_album_view(self):
        """Returns the album view"""
        return self.app.wait_select_single("AlbumViewer",
                                           objectName="albumViewer")

    def get_toolbar_add_button(self):
        """Returns the add button of the tollbar in the events view."""
        return self.get_toolbar_named_toolbarbutton("addButton")

    def get_first_photo(self):
        """Returns the first photo in a newly opened album"""
        return self.app.select_many(
            "FramePortrait", visible=True, isLoaded=True)[0]

    def get_album_photo_view(self):
        """Returns the photo view of the album viewer"""
        view = self.get_album_view()
        return view.select_single("PopupPhotoViewer")

    def number_of_photos(self):
        """Returns the numer of visible photos"""
        return len(self.app.select_many("FramePortrait",
                                        visible=True, isLoaded=True))

    def media_selector_loader(self):
        """Returns the loader for the media selector"""
        return self.app.select_single("QQuickLoader",
                                      objectName="albumMediaSelectorLoader")

    def get_plus_icon_empty_album(self):
        """Returns the plus icon visible in empty albums"""
        return self.app.select_single("QQuickImage",
                                      objectName="addButton", visible=True)

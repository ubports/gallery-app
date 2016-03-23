# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_app.emulators.gallery_utils import GalleryUtils


class MediaSelector(GalleryUtils):
    """A class that makes it easy to interact with the gallery app"""

    def __init__(self, app):
        self.app = app

    def get_media_selector(self):
        """Return the media selector"""
        return self.app.wait_select_single("MediaSelector")

    def get_second_photo(self):
        """Return the second photo item"""
        selector = self.get_media_selector()

        # get the first row in the selector
        mediaRow = selector.wait_select_single(objectName="mediaSelectorList0")

        # get the loader for the second item
        thumbnailLoader = mediaRow.wait_select_single(
            objectName="thumbnailLoader1")

        # return the item itself
        return thumbnailLoader.wait_select_single(objectName="eventPhoto",
                                                  thumbnailLoaded=True)

    def ensure_fully_open(self):
        """Ensure media selector is fully open"""
        media_selector = self.get_media_selector()
        media_selector.opacity.wait_for(1.0)

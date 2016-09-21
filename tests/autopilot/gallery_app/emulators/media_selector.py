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
        events = self.app.select_many('OrganicMediaList')
        photos = events[0].select_many( 'QQuickItem', objectName='eventPhoto')
        # return the item itself
        return photos[0]

    def ensure_fully_open(self):
        """Ensure media selector is fully open"""
        media_selector = self.get_media_selector()
        media_selector.opacity.wait_for(1.0)

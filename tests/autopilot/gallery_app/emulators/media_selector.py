# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils


class MediaSelector(GalleryUtils):
    """An emulator class that makes it easy to interact with the gallery app"""

    def __init__(self, app):
        self.app = app

    def get_media_selector(self):
        """Returns the media selector"""
        return self.select_single_retry("MediaSelector")

    def get_toolbar_add_button(self):
        """Returns the add button of the tollbar in the media selector"""
        return self.get_toolbar_button(0)

    def get_second_photo(self):
        """Returns the second photo item"""
        selector = self.get_media_selector()
        list_view = selector.get_children_by_type("QQuickListView")[0]
        item = list_view.get_children_by_type("QQuickItem")[0]
        media_list = item.get_children_by_type("OrganicMediaList")[0]
        list_view = media_list.get_children_by_type("QQuickListView")[0]
        item = list_view.get_children_by_type("QQuickItem")[0]
        item = item.get_children_by_type("QQuickItem")[3]
        return item.get_children_by_type("UbuntuShape")[0]

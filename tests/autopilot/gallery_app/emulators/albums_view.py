# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils

class AlbumsView(GalleryUtils):

    def __init__(self, app):
        self.app = app

    def get_albums_tab_button(self):
        """Returns the photos tab."""
        return self.app.select_single("AbstractButton", buttonIndex=0)

    def get_toolbar_add_button(self):
        """Returns the add button of the tollbar in the events view."""
        return self.get_toolbar_button(0)

    def number_of_albums_in_albums_view(self):
        #"""Returns the number of albums shown in alsbums overview"""
        albums_delegates = self.app.select_many("CheckerboardDelegate", objectName="albumsOverviewDelegate")
        return len(albums_delegates)

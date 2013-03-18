# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils

class EventsView(GalleryUtils):
    """An emulator class that makes it easy to interact with the event view of gallery."""

    def __init__(self, app):
        self.app = app

    def get_select_icon(self):
        """Returns the select icon of the events view."""
        return self.get_toolbar_button(0)

    def get_delete_icon(self):
        return self.get_toolbar_button(1)

    def get_camera_icon(self):
        """Returns the camera icon of the main view."""
        return self.get_toolbar_button(2)

    def get_album_editor(self):
        """Returns the album editor."""
        return self.app.select_single("AlbumEditor", objectName="mainAlbumEditor")

    def get_events_view_delete_dialog(self):
        """Returns the delete dialog in the events view."""
        return self.app.select_single("DeletePopover", objectName="eventsViewDeletePopover")

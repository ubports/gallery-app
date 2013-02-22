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

    def get_plus_icon(self):
        """Returns the 'plus' icon of the main view."""
        return self.app.select_single("StandardToolbarIconButton", objectName="toolbarPlusIcon")

    def get_camera_icon(self):
        """Returns the camera icon of the main view."""
        # FIXME use another property than the text, as that will change with the translations
        return self.app.select_single("Button", text="Camera")

    def get_select_icon(self):
        """Returns the select icon of the events view."""
        # FIXME use another property than the text, as that will change with the translations
        return self.app.select_single("Button", text="Select")

    def get_delete_icon(self):
        event_bar = self.get_tool_bar()
        item = event_bar.get_children_by_type("QQuickItem")[0]
        row = item.get_children_by_type("QQuickRow")[0]
        return row.get_children_by_type("Button")[1]

    def get_album_editor(self):
        """Returns the album editor."""
        return self.app.select_single("AlbumEditor", objectName="mainAlbumEditor")

    def get_events_view_delete_dialog(self):
        """Returns the delete dialog in the events view."""
        return self.app.select_single("DeletePopover", objectName="eventsViewDeletePopover")

# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


class EventsView(object):
    """An emulator class that makes it easy to interact with the gallery app."""

    def __init__(self, app):
        self.app = app

    def get_qml_view(self):
        """Get the main QML view"""
        return self.app.select_single("QQuickView")

    def get_events_tab(self):
        """Returns the 'Events' tab."""
        return self.app.select_single("GalleryTab", objectName="toolbarEventsTab")

    def get_albums_tab(self):
        """Returns the 'Albums' tab."""
        return self.app.select_single("GalleryTab", objectName="toolbarAlbumsTab")

    def get_plus_icon(self):
        """Returns the 'plus' icon of the main view."""
        return self.app.select_single("StandardToolbarIconButton", objectName="toolbarPlusIcon")

    def get_camera_icon(self):
        """Returns the camera icon of the main view."""
        return self.app.select_single("ChromeButton", objectName="camera")

    def get_select_icon(self):
        """Returns the select icon of the events view."""
        return self.app.select_single("ChromeButton", objectName="select")

    def get_cancel_icon(self):
        """Returns the cancel icon of the events view."""
        return self.app.select_single("ChromeButton", objectName="backButton")

    def get_album_editor(self):
        """Returns the album editor."""
        return self.app.select_single("AlbumEditor", objectName="mainAlbumEditor")

    def get_events_view_chrome_bar(self):
        """Returns the chromebar in the main events view."""
        return self.app.select_single("ChromeBar", objectName="chromeBar")

    def get_events_view_delete_dialog(self):
        """Returns the delete dialog in the events view."""
        return self.app.select_single("DeletePopover", objectName="eventsViewDeletePopover")



# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


class GalleryUtils(object):
    """An emulator class that makes it easy to interact with
    general components of the gallery app.
    """

    def __init__(self, app):
        self.app = app

    def click_item(self, item, pointing_device):
        """Does a mouse click on the passed item, and moved the mouse there before"""
        pointing_device.move_to_object(item)
        pointing_device.click()


    def get_qml_view(self):
        """Get the main QML view"""
        return self.app.select_single("QQuickView")

    def get_main_photo_viewer_loader(self):
        """Returns the loder item for the PhotoViewer."""
        return self.app.select_single("QQuickLoader", objectName="photoViewerLoader")

    def get_main_photo_viewer(self):
        """Returns the PhotoViewer."""
        return self.app.select_single("PhotoViewer", objectName="photoViewer")

    def get_tool_bar(self):
        """Returns the toolbar in the main events view."""
        main_view = self.app.select_single("MainScreen", objectName="overview")
        return main_view.get_children_by_type("Toolbar")[0]

    def get_cancel_icon(self):
        """Returns the cancel icon of the events view."""
        event_bar = self.get_tool_bar()
        item = event_bar.get_children_by_type("QQuickItem")[0]
        return item.get_children_by_type("Button")[0]

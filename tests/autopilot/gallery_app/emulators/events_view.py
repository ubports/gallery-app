# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils


class EventsView(GalleryUtils):
    """An emulator class that makes it easy to interact with the event view of
       gallery."""

    def __init__(self, app):
        self.app = app

    def get_toolbar_camera_button(self):
        """Returns the camera button of the toolbar in the event view"""
        return self.get_toolbar_named_toolbarbutton("cameraButton")

    def get_toolbar_select_button(self):
        """Returns the select button of the toolbar in the event view"""
        return self.get_toolbar_named_toolbarbutton("selectButton")

    def get_toolbar_delete_button(self):
        """Returns the delete button of the toolbar in the event view"""
        return self.get_toolbar_named_toolbarbutton("deleteButton")

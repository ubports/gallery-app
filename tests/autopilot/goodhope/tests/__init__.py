# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""goodhope autopilot tests."""

from os import remove
import os.path

from autopilot.introspection.qt import QtIntrospectionTestMixin
from autopilot.testcase import AutopilotTestCase

from goodhope.emulators.events_view import EventsView
from goodhope.emulators.photo_viewer import PhotoViewer
from goodhope.emulators.album_editor import AlbumEditor
from goodhope.emulators.photos_view import PhotosView


class GoodhopeTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for goodhope tests."""

    def setUp(self):
        super(GoodhopeTestCase, self).setUp()
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            self.launch_test_installed()
        else:
            self.launch_test_local()

    def launch_test_local(self):
        self.app = self.launch_test_application(
            "../../src/gallery", "../../tests/autopilot/goodhope/data/"
            )

    def launch_test_installed(self):
        self.app = self.launch_test_application(
           "gallery", "/usr/lib/python2.7/dist-packages/goodhope/data/"
           )

    @property
    def events_view(self):
        return EventsView(self.app)

    @property
    def photo_viewer(self):
        return PhotoViewer(self.app)

    @property
    def album_editor(self):
        return AlbumEditor(self.app)

    @property
    def photos_view(self):
        return PhotosView(self.app)

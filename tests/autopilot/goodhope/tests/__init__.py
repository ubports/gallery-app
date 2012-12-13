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

from goodhope.emulators.main_window import MainWindow
from goodhope.emulators.photo_viewer import PhotoViewer
from goodhope.emulators.album_editor import AlbumEditor


class GoodhopeTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for goodhope tests."""

    def setUp(self):
        super(GoodhopeTestCase, self).setUp()
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            self.launch_test_installed()
            os.system("cp /usr/lib/python2.7/dist-packages/goodhope/data/sample.jpg ~/Pictures/")
        else:
            self.launch_test_local()
            os.system("cp goodhope/data/sample.jpg ~/Pictures/")

        sample_location = os.path.expanduser("~/Pictures/sample.jpg")

        self.assertTrue(os.path.exists(sample_location))
        self.addCleanup(os.remove, sample_location)


    def launch_test_local(self):
        self.app = self.launch_test_application(
            "../../gallery"
            )

    def launch_test_installed(self):
        self.app = self.launch_test_application(
           "gallery",
           )

    @property
    def main_window(self):
        return MainWindow(self.app)

    @property
    def photo_viewer(self):
        return PhotoViewer(self.app)

    @property
    def album_editor(self):
        return AlbumEditor(self.app)


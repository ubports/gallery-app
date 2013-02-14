# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""gallery autopilot tests."""

from os import remove, system
import os.path
import shutil

from autopilot.introspection.qt import QtIntrospectionTestMixin
from autopilot.testcase import AutopilotTestCase

from gallery.emulators.events_view import EventsView
from gallery.emulators.photo_viewer import PhotoViewer
from gallery.emulators.album_editor import AlbumEditor
from gallery.emulators.photos_view import PhotosView


class GalleryTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for gallery tests."""

    sample_dir = "/tmp/ubuntu-gallery_ap"
    sample_file = "/tmp/ubuntu-gallery/sample.jpg"
    installed_sample_file = "/usr/lib/python2.7/dist-packages/ubuntu-gallery/data/sample.jpg"
    local_sample_file = "gallery/data/sample.jpg"

    def setUp(self):
        super(GalleryTestCase, self).setUp()

        os.makedirs(self.sample_dir)
        self.assertTrue(os.path.exists(self.sample_dir))
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            shutil.copy(self.installed_sample_file, self.sample_dir)
            self.assertTrue(os.path.isfile(self.sample_file))
            self.launch_test_installed()
        else:
            shutil.copy(self.local_sample_file, self.sample_dir)
            self.assertTrue(os.path.isfile(self.sample_file))
            self.launch_test_local()

        self.addCleanup(shutil.rmtree, self.sample_dir)

    def launch_test_local(self):
        self.app = self.launch_test_application(
            "../../src/ubuntu-gallery", self.sample_dir
            )

    def launch_test_installed(self):
        self.app = self.launch_test_application(
           "ubuntu-gallery", self.sample_dir
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
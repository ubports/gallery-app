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
from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from gallery_app.emulators.album_editor import AlbumEditor
from gallery_app.emulators.events_view import EventsView
from gallery_app.emulators.photo_viewer import PhotoViewer
from gallery_app.emulators.photos_view import PhotosView


class GalleryTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for gallery tests."""

    sample_dir = "/tmp/gallery-ap_sd"
    sample_file = sample_dir + "/sample.jpg"
    installed_sample_file = "/usr/lib/python2.7/dist-packages/gallery_app/data/sample.jpg"
    local_sample_file = "gallery_app/data/sample.jpg"

    def setUp(self):
        super(GalleryTestCase, self).setUp()

        if (not os.path.exists(self.sample_dir)):
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
            "../../src/gallery-app", self.sample_dir
            )

    def launch_test_installed(self):
        self.app = self.launch_test_application(
           "gallery-app", self.sample_dir
           )

    def click_item(self, item):
        """Does a mouse click on the passed item, and moved the mouse there before"""
        self.events_view.click_item(item, self.pointing_device)

    def reveal_tool_bar(self):
        toolbar = self.events_view.get_tool_bar()

        if toolbar.active:
            # Toolbar already open
            return

        main_view = self.events_view.get_qml_view()
        x, y, w, h = toolbar.globalRect
        x_line = main_view.x + main_view.width * 0.5
        start_y = main_view.y + main_view.height - 1
        stop_y = start_y - 2*h

        self.pointing_device.drag(x_line, start_y, x_line, stop_y)
        self.assertThat(toolbar.active, Eventually(Equals(True)))

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

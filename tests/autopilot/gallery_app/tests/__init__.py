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
from testtools.matchers import Equals, GreaterThan
from autopilot.matchers import Eventually

from gallery_app.emulators.gallery_utils import GalleryUtils

from time import sleep

class GalleryTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for gallery tests."""

    sample_dir = "/tmp/gallery-ap_sd"
    sample_file = sample_dir + "/sample01.jpg"
    sample_file_source = "/sample01.jpg"
    installed_sample_dir = "/usr/lib/python2.7/dist-packages/gallery_app/data"
    local_sample_dir = "gallery_app/data"

    @property
    def gallery_utils(self):
        return GalleryUtils(self.app)

    def setUp(self):
        super(GalleryTestCase, self).setUp()

        if (os.path.exists(self.sample_dir)):
            shutil.rmtree(self.sample_dir)
        self.assertFalse(os.path.exists(self.sample_dir))
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            shutil.copytree(self.installed_sample_dir, self.sample_dir)
            self.assertTrue(os.path.isfile(self.sample_file))
            self.sample_file_source = self.installed_sample_dir + self.sample_file_source
            self.launch_test_installed()
        else:
            shutil.copytree(self.local_sample_dir, self.sample_dir)
            self.assertTrue(os.path.isfile(self.sample_file))
            self.sample_file_source = self.local_sample_dir + self.sample_file_source
            self.launch_test_local()

        """ This is needed to wait for the application to start.
        In the testfarm, the application may take some time to show up."""
        self.assertThat(self.gallery_utils.get_qml_view().visible, Eventually(Equals(True)))

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
        self.pointing_device.move_to_object(item)
        self.pointing_device.click()

    def tap_item(self, item):
        """Does a long mouse press on the passed item, and moved the mouse there before"""
        self.pointing_device.move_to_object(item)
        self.pointing_device.press()
        sleep(1)
        self.pointing_device.release()

    def reveal_toolbar(self):
        toolbar = self.gallery_utils.get_toolbar()
        self.assertThat(toolbar.animating, Eventually(Equals(False)))

        if toolbar.active:
            # Toolbar already open
            return

        main_view = self.gallery_utils.get_qml_view()
        x, y, w, h = toolbar.globalRect
        x_line = main_view.x + main_view.width * 0.5
        start_y = main_view.y + main_view.height - 1
        stop_y = start_y - 2*h

        self.pointing_device.drag(x_line, start_y, x_line, stop_y)
        self.assertThat(toolbar.active, Eventually(Equals(True)))

    def switch_to_albums_tab(self):
        tabs_bar = self.gallery_utils.get_tabs_bar()
        self.click_item(tabs_bar)

        albums_tab_button = self.gallery_utils.get_albums_tab_button()
        #Due to some timing issues sometimes mouse moves to the location a bit earlier
        #even though the tab item is not fully visible, hence the tab does not activate.
        self.assertThat(albums_tab_button.opacity, Eventually(GreaterThan(0.2)))
        self.click_item(albums_tab_button)

        """FIXME find a (functional) way to test if the tabs still move"""
        sleep(1)

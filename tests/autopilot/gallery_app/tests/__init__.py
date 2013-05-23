# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""gallery autopilot tests."""

import os.path
import shutil

from autopilot.input import Mouse, Touch, Pointer
from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from testtools.matchers import Equals, GreaterThan

from gallery_app.emulators.gallery_utils import GalleryUtils

from time import sleep


class GalleryTestCase(AutopilotTestCase):

    """A common test case class that provides several useful methods for
       gallery tests."""

    if model() == 'Desktop':
        scenarios = [
        ('with mouse', dict(input_device_class=Mouse)),
        ]
    else:
        scenarios = [
        ('with touch', dict(input_device_class=Touch)),
        ]

    sample_dir = "/tmp/gallery-ap_sd"
    sample_file = sample_dir + "/sample01.jpg"
    sample_file_source = "/sample01.jpg"
    installed_sample_dir = "/usr/lib/python2.7/dist-packages/gallery_app/data"
    local_sample_dir = "gallery_app/data"
    tap_press_time = 1

    local_location = "../../src/gallery-app"

    @property
    def gallery_utils(self):
        return GalleryUtils(self.app)

    def setUp(self):
        self.pointing_device = Pointer(self.input_device_class.create())
        super(GalleryTestCase, self).setUp()

        if (os.path.exists(self.sample_dir)):
            shutil.rmtree(self.sample_dir)
        self.assertFalse(os.path.exists(self.sample_dir))
        # Lets assume we are installed system wide if this file is somewhere
        # in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            shutil.copytree(self.installed_sample_dir, self.sample_dir)
            self.assertTrue(os.path.isfile(self.sample_file))
            self.sample_file_source = self.installed_sample_dir + \
                self.sample_file_source
            self.launch_test_installed()
        else:
            shutil.copytree(self.local_sample_dir, self.sample_dir)
            self.assertTrue(os.path.isfile(self.sample_file))
            self.sample_file_source = self.local_sample_dir + \
                self.sample_file_source
            self.launch_test_local()

        self.addCleanup(shutil.rmtree, self.sample_dir)

        """ This is needed to wait for the application to start.
        In the testfarm, the application may take some time to show up."""
        self.assertThat(self.gallery_utils.get_qml_view().visible,
                        Eventually(Equals(True)))
        """Wait for the data to be loaded and displayed"""
        self.ensure_at_least_one_event()
        """FIXME somehow on the server gallery sometimes is not fully started
        for switching to the albums view. Therefore this hack of a second"""
        sleep(1)

    def launch_test_local(self):
        self.app = self.launch_test_application(self.local_location,
            self.sample_dir)

    def launch_test_installed(self):
        if model() == 'Desktop':
            self.app = self.launch_test_application("gallery-app",
                self.sample_dir)
        else:
            self.app = self.launch_test_application("gallery-app",
                "--desktop_file_hint=/usr/share/applications/gallery-app.desktop",
                self.sample_dir,
                app_type='qt')

    def ui_update(self):
        """ Gives the program the time to update the UI"""
        sleep(0.1)

    def click_item(self, item):
        """Does a mouse click on the passed item, and moved the mouse there
           before"""
        self.pointing_device.move_to_object(item)
        if model() == 'Desktop':
            sleep(0.25)
        self.pointing_device.click()

    def tap_item(self, item):
        """Does a long mouse press on the passed item, and moved the mouse
           there before"""
        self.pointing_device.move_to_object(item)
        self.pointing_device.click(1, self.tap_press_time)

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
        stop_y = start_y - 2 * h

        self.pointing_device.drag(x_line, start_y, x_line, stop_y)
        self.assertThat(toolbar.state, Eventually(Equals("spread")))

    def ensure_at_least_one_event(self):
        """The event view has to have at least one event
        In case gallery is not yet fully loaded wait a while and test again"""
        num_events = self.gallery_utils.number_of_events()
        if num_events < 1:
            sleep(1)
            num_events = self.gallery_utils.number_of_events()
        self.assertThat(num_events, Equals(1))

    def switch_to_albums_tab(self):
        tabs_bar = self.gallery_utils.get_tabs_bar()
        self.click_item(tabs_bar)

        albums_tab_button = self.gallery_utils.get_albums_tab_button()
        # Due to some timing issues sometimes mouse moves to the location a bit
        # earlier. Even though the tab item is not fully visible, hence the tab
        # does not activate.
        self.assertThat(albums_tab_button.opacity,
                        Eventually(GreaterThan(0.35)))
        self.click_item(albums_tab_button)

        albums_loader = self.gallery_utils.get_albums_viewer_loader()
        self.assertThat(albums_loader.progress, Eventually(Equals(1)))

        # The next check assumes that at least one album is available
        # Check if the albums are availabe - they need some time to load.
        self.assertThat(lambda: len(self.gallery_utils.get_all_albums()),
                        Eventually(GreaterThan(0)))

        # FIXME find a (functional) way to test if the tabs still move
        sleep(1)

    def open_first_album(self):
        first_album = self.album_view.get_first_album()
        self.click_item(first_album)
        self.ensure_view_is_fully_open()

    def ensure_view_is_fully_open(self):
        animated_view = self.album_view.get_animated_album_view()
        self.assertThat(animated_view.isOpen, Eventually(Equals(True)))
        view = self.album_view.get_album_view()
        self.assertThat(view.visible, Eventually(Equals(True)))
        self.assertThat(animated_view.animationRunning,
                        Eventually(Equals(False)))

    def ensure_edit_dialog_visible(self):
        edit_dialog = self.photo_viewer.get_photo_edit_dialog()
        self.assertThat(edit_dialog.opacity, (Eventually(Equals(1))))

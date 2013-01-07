# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests for the Gallery App"""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from goodhope.tests import GoodhopeTestCase


class TestMainWindow(GoodhopeTestCase):
    """Tests the main gallery features"""

    """ This is needed to wait for the application to start.
        In the testfarm, the application may take some time to show up."""
    def setUp(self):
        super(TestMainWindow, self).setUp()
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def tearDown(self):
        super(TestMainWindow, self).tearDown()

    def test_chrome_bar_reveals_main_view(self):
        """Makes sure the chromebar at the bottom is revealed when its dragged
        upwards.

        """
        chromebar = self.app.select_single("ChromeBar", objectName="chromeBar")
        self.pointing_device.move_to_object(chromebar)

        x, y, w, h = chromebar.globalRect

        self.pointing_device.press()
        self.pointing_device.move(x + (w/2), y + (h/2 - 50))
        self.pointing_device.release()

        self.assertThat(chromebar.showChromeBar, Eventually(Equals(True)))

    def test_camera_icon_hover(self):
        """Ensures that when the mouse is over the camera icon it has the
        hovered state.

        """
        camera_icon = self.main_window.get_camera_icon()

        self.pointing_device.move_to_object(camera_icon)

        self.assertThat(camera_icon.hovered, Eventually(Equals(True)))


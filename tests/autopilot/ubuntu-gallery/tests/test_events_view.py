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

from gallery.tests import GalleryTestCase


class TestEventsView(GalleryTestCase):
    """Tests the main gallery features"""

    """ This is needed to wait for the application to start.
        In the testfarm, the application may take some time to show up."""
    def setUp(self):
        super(TestEventsView, self).setUp()
        self.assertThat(self.events_view.get_qml_view().visible, Eventually(Equals(True)))

    def tearDown(self):
        super(TestEventsView, self).tearDown()

    def reveal_chrome_bar(self):
        chromebar = self.events_view.get_events_view_chrome_bar()
        self.pointing_device.move_to_object(chromebar)

        x, y, w, h = chromebar.globalRect

        self.pointing_device.press()
        self.pointing_device.move(x + (w/2), y + (h/2 - 50))
        self.pointing_device.release()

        self.assertThat(chromebar.showChromeBar, Eventually(Equals(True)))

    def click_select_icon(self):
        select_icon = self.events_view.get_select_icon()
        self.pointing_device.move_to_object(select_icon)
        self.pointing_device.click()

    def test_camera_icon_hover(self):
        """Ensures that when the mouse is over the camera icon it has the
        hovered state.

        """
        self.reveal_chrome_bar()

        camera_icon = self.events_view.get_camera_icon()
        self.pointing_device.move_to_object(camera_icon)

        self.assertThat(camera_icon.hovered, Eventually(Equals(True)))

    def test_select_button_cancel(self):
        """Clicking the cancel button after clicking the select button must
        hide the chromebar automatically.

        """
        self.reveal_chrome_bar()
        self.click_select_icon()

        cancel_icon = self.events_view.get_cancel_icon()
        self.pointing_device.move_to_object(cancel_icon)
        self.pointing_device.click()

        chromebar = self.events_view.get_events_view_chrome_bar()
        self.assertThat(chromebar.showChromeBar, Eventually(Equals(False)))

    def test_delete_glows_on_selecting_a_photo(self):
        """Selecting a photo must make the delete button clickable."""
        self.reveal_chrome_bar()
        self.click_select_icon()

        first_photo = self.photo_viewer.get_first_image_in_photo_viewer()
        self.pointing_device.move_to_object(first_photo)
        self.pointing_device.click()

        trash_button = self.photo_viewer.get_viewer_chrome_trash_button()
        delete_dialog = self.events_view.get_events_view_delete_dialog()

        self.pointing_device.move_to_object(trash_button)
        self.pointing_device.click()

        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))



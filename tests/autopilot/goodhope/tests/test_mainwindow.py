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

    def click_albums_tab(self):
        albums_tab = self.main_window.get_albums_tab()

        self.mouse.move_to_object(albums_tab)
        self.mouse.click()

    def click_plus_icon(self):
        add_icon = self.main_window.get_plus_icon()

        self.mouse.move_to_object(add_icon)
        self.mouse.click()

    def test_events_tab_focus(self):
        """When the app is started 'Events' tab must be selected."""
        events_tab = self.main_window.get_events_tab()

        self.assertThat(events_tab.state, Eventually(Equals("selected")))

    def test_albums_tab_focus(self):
        """When the 'Albums' tab is clicked it should be selected."""
        albums_tab = self.main_window.get_albums_tab()

        self.mouse.move_to_object(albums_tab)
        self.mouse.click()

        self.assertThat(albums_tab.state, Eventually(Equals("selected")))

    def test_plus_icon_hover(self):
        """Ensures that when the mouse is over the 'plus' icon it has the
        hovered state.

        """
        add_icon = self.main_window.get_plus_icon()

        self.mouse.move_to_object(add_icon)

        self.assertThat(add_icon.hovered, Eventually(Equals(True)))

    def test_camera_icon_hover(self):
        """Ensures that when the mouse is over the camera icon it has the
        hovered state.

        """
        camera_icon = self.main_window.get_camera_icon()

        self.mouse.move_to_object(camera_icon)

        self.assertThat(camera_icon.hovered, Eventually(Equals(True)))

    def test_plus_icon_click(self):
        """Clicking on the plus icon must open the album editor."""
        album_editor = self.main_window.get_album_editor()

        self.click_albums_tab()
        self.click_plus_icon()

        self.assertThat(album_editor.opacity, Eventually(Equals(1.0)))



# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the albums view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, GreaterThan
from autopilot.matchers import Eventually
from autopilot.platform import model

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.albums_view import AlbumsView
from gallery_app.emulators import album_editor

from time import sleep
from os import environ as env

class TestAlbumsView(GalleryTestCase):
    envDesktopMode = None

    @property
    def albums_view(self):
        return AlbumsView(self.app)

    def setUp(self):
        self.ARGS = []

        self.envDesktopMode = env.get("DESKTOP_MODE")

        if model() == "Desktop":
            env["DESKTOP_MODE"] = "1"
        else:
            env["DESKTOP_MODE"] = "0"

        super(TestAlbumsView, self).setUp()
        self.switch_to_albums_tab()

    def tearDown(self):
        if self.envDesktopMode:
            env["DESKTOP_MODE"] = self.envDesktopMode
        else:
            del env["DESKTOP_MODE"]

        super(TestAlbumsView, self).tearDown()

    def test_add_album(self):
        """Add one album, and checks if the number of albums went up by one"""
        albums = self.albums_view.number_of_albums_in_albums_view()
        self.main_view.open_toolbar().click_button("addButton")
        self.assertThat(lambda: self.albums_view.number_of_albums_in_albums_view(),
                        Eventually(Equals(albums+1)))

    def test_add_album_and_cancel(self):
        """Add one album, cancel it and checks if the number of albums does not change"""
        albums = self.albums_view.number_of_albums_in_albums_view()
        self.main_view.open_toolbar().click_button("addButton")
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        editor.ensure_fully_open()
        self.main_view.get_toolbar().click_custom_button("cancelButton")
        self.assertThat(lambda: self.albums_view.number_of_albums_in_albums_view(),
                        Eventually(Equals(albums)))

    # Check if Camera Button is not visible at Desktop mode
    def test_camera_button_visible(self):
        self.main_view.open_toolbar()
        toolbar = self.main_view.get_toolbar()
        cameraButton = toolbar.select_single("ActionItem", objectName="cameraButton")
        if model() == "Desktop":
            self.assertThat(cameraButton.visible, Equals(False))
        else:
            self.assertThat(cameraButton.visible, Equals(True))


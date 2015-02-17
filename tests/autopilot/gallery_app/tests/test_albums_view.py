# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the albums view of the gallery app."""

from testtools.matchers import Equals
from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.introspection.dbus import StateNotFoundError

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.album_view import AlbumView
from gallery_app.emulators.albums_view import AlbumsView
from gallery_app.emulators.media_selector import MediaSelector
from gallery_app.emulators import album_editor

from os import environ as env


class TestAlbumsView(GalleryTestCase):
    envDesktopMode = None

    @property
    def album_view(self):
        return AlbumView(self.app)

    @property
    def albums_view(self):
        return AlbumsView(self.app)

    @property
    def media_selector(self):
        return MediaSelector(self.app)

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

    def check_header_button_exist(self, button):
        header = self.main_view.get_header()
        buttonName = button + "_header_button"
        try:
            header.select_single(objectName=buttonName)
        except StateNotFoundError:
            return False
        return True

    def test_add_album(self):
        """Add one album, and checks if the number of albums went up by one"""
        albums = self.albums_view.number_of_albums_in_albums_view()
        self.main_view.get_header().click_action_button("addButton")
        # a photo must be added to the album for it to be saved
        plus = self.album_view.get_plus_icon_empty_album()
        self.click_item(plus)
        self.media_selector.ensure_fully_open()
        photo = self.media_selector.get_second_photo()
        checkbox = photo.select_single(objectName="selectionCheckbox")
        self.click_item(checkbox)
        self.main_view.get_header().click_action_button("addButton")

        self.assertThat(
            lambda: self.albums_view.number_of_albums_in_albums_view(),
            Eventually(Equals(albums+1))
        )

    def test_add_album_and_cancel(self):
        """Add one album, cancel it and checks if the number of albums does not
        change. Do this in the two different ways we have to cancel editing.
        """

        # Cancel by pressing back in the header
        albums = self.albums_view.number_of_albums_in_albums_view()
        self.main_view.get_header().click_action_button("addButton")
        editor = self.app.select_single(album_editor.AlbumEditor)
        editor.ensure_fully_open()
        self.main_view.get_header().click_custom_back_button()
        self.assertThat(
            lambda: self.albums_view.number_of_albums_in_albums_view(),
            Eventually(Equals(albums))
        )

        # Cancel by clicking outside of the album cover
        self.main_view.get_header().click_action_button("addButton")
        editor = self.app.select_single(album_editor.AlbumEditor)
        editor.ensure_fully_open()
        editor.close()

        self.assertThat(
            lambda: self.albums_view.number_of_albums_in_albums_view(),
            Eventually(Equals(albums))
        )

    # Check if Camera Button is not visible at Desktop mode
    def test_camera_button_visible(self):
        cameraButtonVisible = self.check_header_button_exist("cameraButton")
        if model() == "Desktop":
            self.assertThat(cameraButtonVisible, Equals(False))
        else:
            self.assertThat(cameraButtonVisible, Equals(True))

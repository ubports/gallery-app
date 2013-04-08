# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the album view of the gallery app"""

from __future__ import absolute_import

from testtools.matchers import Equals
from autopilot.matchers import Eventually

from gallery_app.emulators.album_view import AlbumView
from gallery_app.tests import GalleryTestCase


class TestAlbumView(GalleryTestCase):
    """Tests the album view of the gallery app"""

    @property
    def album_view(self):
        return AlbumView(self.app)

    def setUp(self):
        super(TestAlbumView, self).setUp()
        self.switch_to_albums_tab()
        self.open_first_album()

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

    def test_album_view_open_photo(self):
        photo = self.album_view.get_first_photo()
        self.click_item(photo)
        photo_view = self.album_view.get_album_photo_view()
        self.assertThat(photo_view.visible, Eventually(Equals(True)))
        self.assertThat(photo_view.isPoppedUp, Eventually(Equals(True)))

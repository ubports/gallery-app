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
from gallery_app.emulators.albums_view import AlbumsView
from gallery_app.emulators.media_selector import MediaSelector
from gallery_app.tests import GalleryTestCase

from time import sleep


class TestAlbumView(GalleryTestCase):
    """Tests the album view of the gallery app"""

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
        super(TestAlbumView, self).setUp()
        self.switch_to_albums_tab()

    def ensure_media_selector_is_fully_open(self):
        media_selector = self.media_selector.get_media_selector()
        self.assertThat(media_selector.opacity, Eventually(Equals(1.0)))

    def ensure_media_selector_is_fully_closed(self):
        loader = self.album_view.media_selector_loader()
        self.assertThat(loader.status, Eventually(Equals(0)))

    def test_album_view_open_photo(self):
        self.open_first_album()
        photo = self.album_view.get_first_photo()
        self.click_item(photo)
        photo_view = self.album_view.get_album_photo_view()
        self.assertThat(photo_view.visible, Eventually(Equals(True)))
        self.assertThat(photo_view.isPoppedUp, Eventually(Equals(True)))

    def test_add_photo(self):
        self.open_first_album()
        num_photos_start = self.album_view.number_of_photos()
        self.assertThat(num_photos_start, Equals(1))

        # open media selector but cancel
        self.reveal_toolbar()
        add_button = self.album_view.get_toolbar_add_button()
        self.click_item(add_button)
        self.ensure_media_selector_is_fully_open()

        cancel = self.media_selector.get_toolbar_cancel_icon()
        self.click_item(cancel)
        self.ensure_media_selector_is_fully_closed()

        num_photos = self.album_view.number_of_photos()
        self.assertThat(num_photos, Equals(num_photos_start))

        # open media selector and add a photo
        self.reveal_toolbar()
        add_button = self.album_view.get_toolbar_add_button()
        self.click_item(add_button)
        self.ensure_media_selector_is_fully_open()

        photo = self.media_selector.get_second_photo()
        self.click_item(photo)
        add_button = self.media_selector.get_toolbar_add_button()
        self.click_item(add_button)

        num_photos = self.album_view.number_of_photos()
        self.assertThat(num_photos, Equals(num_photos_start + 1))

    def test_add_photo_to_new_album(self):
        self.reveal_toolbar()
        add_button = self.albums_view.get_toolbar_add_button()
        self.click_item(add_button)
        self.ui_update()

        self.open_first_album()
        num_photos_start = self.album_view.number_of_photos()
        self.assertThat(num_photos_start, Equals(0))

        plus = self.album_view.get_plus_icon_empty_album()
        self.click_item(plus)
        self.ensure_media_selector_is_fully_open()

        photo = self.media_selector.get_second_photo()
        self.click_item(photo)
        add_button = self.media_selector.get_toolbar_add_button()
        #Not using click_item() here as this test fails with the
        #short delay between click, using 0.5 sec wait here.
        self.pointing_device.move_to_object(add_button)
        sleep(0.5)
        self.pointing_device.click()

        num_photos = self.album_view.number_of_photos()
        self.assertThat(num_photos, Equals(num_photos_start + 1))

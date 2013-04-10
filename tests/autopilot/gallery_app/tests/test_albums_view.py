# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the albums view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.albums_view import AlbumsView

from time import sleep


class TestPhotosView(GalleryTestCase):

    @property
    def albums_view(self):
        return AlbumsView(self.app)

    def setUp(self):
        super(TestPhotosView, self).setUp()
        self.switch_to_albums_tab()

    def compare_number_of_albums(self, target):
        """Test if the number of albums is correct. For robustness (timing
           issues), the test is repeated after one second in case it fails"""
        num_of_albums = self.albums_view.number_of_albums_in_albums_view()
        if num_of_albums != target:
            sleep(1)
            num_of_albums = self.albums_view.number_of_albums_in_albums_view()
        self.assertThat(num_of_albums, Equals(target))

    def test_add_album(self):
        """Add one album, ench checks if the bumber of albums went from 1 to
           2"""
        self.compare_number_of_albums(1)

        self.reveal_toolbar()
        add_button = self.albums_view.get_toolbar_add_button()
        self.click_item(add_button)

        self.compare_number_of_albums(2)

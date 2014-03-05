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

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.albums_view import AlbumsView

from time import sleep


class TestAlbumsView(GalleryTestCase):

    @property
    def albums_view(self):
        return AlbumsView(self.app)

    def setUp(self):
        self.ARGS = []
        super(TestAlbumsView, self).setUp()
        self.switch_to_albums_tab()

    def test_add_album(self):
        """Add one album, and checks if the number of albums went up by one"""
        albums = self.albums_view.number_of_albums_in_albums_view()
        self.main_view.open_toolbar().click_button("addButton")
        self.assertThat(lambda: self.albums_view.number_of_albums_in_albums_view(),
                        Eventually(Equals(albums+1)))

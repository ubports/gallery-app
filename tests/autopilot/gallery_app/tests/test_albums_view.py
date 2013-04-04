# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the albums view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals, GreaterThan
from autopilot.matchers import Eventually

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


    def test_add_album(self):
        num_of_albums = self.albums_view.number_of_albums_in_albums_view()
        self.assertThat(num_of_albums, Equals(1))

        self.reveal_toolbar()
        add_button = self.albums_view.get_toolbar_add_button()
        self.click_item(add_button)

        # no need to wait for the end of the animation, but a draw update is needed - 100ms for sure is enough
        sleep(0.1)

        num_of_albums = self.albums_view.number_of_albums_in_albums_view()
        self.assertThat(num_of_albums, Equals(2))

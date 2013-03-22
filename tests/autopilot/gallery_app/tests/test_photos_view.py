# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photos view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from gallery_app.tests import GalleryTestCase

from time import sleep


class TestPhotosView(GalleryTestCase):

    def setUp(self):
        super(TestPhotosView, self).setUp()
        self.assertThat(self.photo_viewer.get_qml_view().visible, Eventually(Equals(True)))

        self.click_tabs_bar()
        self.click_photos_tab_button()

        photos_view = self.photos_view.get_photos_view()
        self.assertThat(photos_view.focus, Eventually(Equals(True)))
        sleep(1)

    def click_tabs_bar(self):
        tabs_bar = self.photos_view.get_tabs_bar()
        self.click_item(tabs_bar)

    def click_photos_tab_button(self):
        photos_tab_button = self.app.select_single("AbstractButton", buttonIndex=5)

        #Due to some timing issues sometimes mouse moves to the location a bit earlier
        #even though the tab item is not fully visible, hence the tab does not activate.
        self.assertTrue(photos_tab_button.opacity, Eventually(Equals("=<0.2")))

        photos_tab = self.photos_view.get_photos_tab()
        self.click_item(photos_tab)

    def click_first_photo(self):
        photo = self.photos_view.get_first_photo_in_photos_view()
        self.click_item(photo)


    def test_open_photo(self):
        self.click_first_photo()
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Album editor of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from goodhope.tests import GoodhopeTestCase


class TestPhotoViewer(GoodhopeTestCase):

    def setUp(self):
        super(TestPhotoViewer, self).setUp()

        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def click_first_photo(self):
        single_photo = self.photo_viewer.get_first_image_in_photo_viewer()

        self.mouse.move_to_object(single_photo)
        self.mouse.click()

    # def test_photo_viewer_shows(self):
    #     self.click_first_photo()

    #     photo_viewer = self.photo_viewer.get_photo_viewer()
    #     photo_viewer_chrome = self.photo_viewer.get_photo_viewer_chrome()

    #     self.assertThat(photo_viewer.visible, Eventually(Equals(True)))
    #     self.assertThat(photo_viewer_chrome.visible, Eventually(Equals(True)))

    def test_delete_buttons_deletes_picture(self):
        self.click_first_photo()
        photo_viewer = self.photo_viewer.get_photo_viewer()


        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

        trash_button = self.photo_viewer.get_viewer_chrome_trash_icon()

        self.mouse.move_to_object(trash_button)

        self.mouse.click()




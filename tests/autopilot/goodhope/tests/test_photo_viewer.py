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

from time import sleep


class TestPhotoViewer(GoodhopeTestCase):

    def setUp(self):
        super(TestPhotoViewer, self).setUp()
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

        self.click_first_photo()

        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

        photo_viewer_chrome = self.photo_viewer.get_photo_viewer_chrome()

        if photo_viewer_chrome.visible is not True:
            self.mouse.move_to_object(photo_viewer)
            self.mouse.click()

        self.assertThat(photo_viewer_chrome.visible, Eventually(Equals(True)))

    def click_first_photo(self):
        single_photo = self.photo_viewer.get_first_image_in_photo_viewer()

        self.mouse.move_to_object(single_photo)
        self.mouse.click()

    def test_nav_bar_back_button(self):
        """Clicking the back button must close the photo."""
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        back_button = self.photo_viewer.get_viewer_chrome_back_button()

        self.mouse.move_to_object(back_button)
        self.mouse.click()

        self.assertThat(photo_viewer.visible, Eventually(Equals(False)))

    def test_nav_bar_trash_button(self):
        """Clicking the trash button must show the delete dialog."""
        trash_button = self.photo_viewer.get_viewer_chrome_trash_button()
        delete_dialog = self.photo_viewer.get_delete_dialog()

        self.mouse.move_to_object(trash_button)
        self.assertThat(trash_button.hovered, Eventually(Equals(True)))
        self.mouse.click()

        # we are not testing actual deletion due to a crash (lp:1083958)
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

    def test_nav_bar_album_picker_button(self):
        """Clicking the album picker must show the picker dialog."""
        album_button = self.photo_viewer.get_viewer_chrome_album_button()
        album_picker = self.photo_viewer.get_popup_album_picker()

        self.mouse.move_to_object(album_button)
        self.assertThat(album_button.hovered, Eventually(Equals(True)))
        self.mouse.click()

        self.assertThat(album_picker.visible, Eventually(Equals(True)))

    def test_nav_bar_share_button(self):
        """Clicking the share button must show the share dialog."""
        share_button = self.photo_viewer.get_viewer_chrome_share_button()
        share_menu = self.photo_viewer.get_share_dialog()

        self.mouse.move_to_object(share_button)
        self.assertThat(share_button.hovered, Eventually(Equals(True)))
        self.mouse.click()

        self.assertThat(share_menu.visible, Eventually(Equals(True)))

    def test_nav_bar_more_button(self):
        """Click 'more' button must show more options."""
        more_button = self.photo_viewer.get_viewer_chrome_more_button()
        more_menu = self.photo_viewer.get_more_dialog()

        self.mouse.move_to_object(more_button)
        self.assertThat(more_button.hovered, Eventually(Equals(True)))
        self.mouse.click()

        self.assertThat(more_menu.visible, Eventually(Equals(True)))

    def test_nav_bar_edit_button(self):
        """Clicking the edit button must show the edit dialog."""
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button()
        edit_dialog = self.photo_viewer.get_photo_edit_dialog()

        self.mouse.move_to_object(edit_button)
        self.assertThat(edit_button.hovered, Eventually(Equals(True)))
        self.mouse.click()

        self.assertThat(edit_dialog.visible, Eventually(Equals(True)))

    def test_double_click_zoom(self):
        """Double clicking an opened photo must zoom it."""
        opened_photo = self.photo_viewer.get_photo_component()

        self.mouse.move_to_object(opened_photo)
        self.mouse.click()
        self.mouse.click()

        self.assertThat(opened_photo.fullyZoomed, Eventually(Equals(True)))

        self.mouse.click()
        self.mouse.click()

        self.assertThat(opened_photo.fullyUnzoomed, Eventually(Equals(True)))

    def test_photo_crop_box_shows(self):
        """Clicking the crop item in the edit dialog must show crop interactor."""
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button()
        crop_item = self.photo_viewer.get_crop_menu_item()
        crop_box = self.photo_viewer.get_crop_interactor()

        self.mouse.move_to_object(edit_button)
        self.mouse.click()

        self.mouse.move_to_object(crop_item)
        self.mouse.click()

        self.assertThat(crop_box.visible, Eventually(Equals(True)))










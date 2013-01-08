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

from os.path import exists, expanduser
import os


class TestPhotoViewer(GoodhopeTestCase):

    def setUp(self):
        super(TestPhotoViewer, self).setUp()
        self.assertThat(self.events_view.get_qml_view().visible, Eventually(Equals(True)))

        self.click_first_photo()

        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

        photo_viewer_chrome = self.photo_viewer.get_photo_viewer_chrome()

        if photo_viewer_chrome.showChromeBar == False:
            self.pointing_device.move_to_object(photo_viewer)
            self.pointing_device.click()

        self.assertThat(photo_viewer_chrome.showChromeBar, Eventually(Equals(True)))

    def click_first_photo(self):
        single_photo = self.photo_viewer.get_first_image_in_photo_viewer()

        self.pointing_device.move_to_object(single_photo)
        self.pointing_device.click()

    def test_nav_bar_back_button(self):
        """Clicking the back button must close the photo."""
        photo_viewer = self.photo_viewer.get_main_photo_viewer()

        back_button = self.photo_viewer.get_viewer_chrome_back_button()

        self.pointing_device.move_to_object(back_button)
        self.pointing_device.click()

        self.assertThat(photo_viewer.visible, Eventually(Equals(False)))

    def test_nav_bar_trash_button(self):
        """Clicking the trash button must show the delete dialog."""
        trash_button = self.photo_viewer.get_viewer_chrome_trash_button()
        delete_dialog = self.photo_viewer.get_delete_dialog()

        self.pointing_device.move_to_object(trash_button)
        self.assertThat(trash_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        cancel_item = self.photo_viewer.get_delete_popover_cancel_item()
        self.pointing_device.move_to_object(cancel_item)
        self.pointing_device.click()

        self.assertThat(delete_dialog.visible, Eventually(Equals(False)))

        self.pointing_device.move_to_object(trash_button)
        self.assertThat(trash_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        delete_item = self.photo_viewer.get_delete_popover_delete_item()
        self.pointing_device.move_to_object(delete_item)
        self.pointing_device.click()

        self.assertThat(lambda: exists(expanduser("~/Pictures/sample.jpg")),
                                                    Eventually(Equals(False)))

        #Up until the last line above we are testing if the file got delete, now here we
        #are re-copying the sample.jpg file because it seems the addCleanup method in __init__
        #tries to remove the sample.jpg but if it does not find it the test fails. So this
        #"hack" saves us from that. --om26er
        if exists(expanduser("/usr/lib/python2.7/dist-packages/goodhope/data/sample.jpg")):
            os.system("cp /usr/lib/python2.7/dist-packages/goodhope/data/sample.jpg ~/Pictures/")
        else:
            os.system("cp goodhope/data/sample.jpg ~/Pictures/")

        self.assertThat(lambda: exists(expanduser("~/Pictures/sample.jpg")),
                                                    Eventually(Equals(True)))


    # def test_nav_bar_album_picker_button(self):
    #     """Clicking the album picker must show the picker dialog."""
    #     album_button = self.photo_viewer.get_viewer_chrome_album_button()
    #     album_picker = self.photo_viewer.get_popup_album_picker()

    #     self.pointing_device.move_to_object(album_button)
    #     self.assertThat(album_button.hovered, Eventually(Equals(True)))
    #     self.pointing_device.click()

    #     self.assertThat(album_picker.visible, Eventually(Equals(True)))

    def test_nav_bar_share_button(self):
        """Clicking the share button must show the share dialog."""
        share_button = self.photo_viewer.get_viewer_chrome_share_button()
        share_menu = self.photo_viewer.get_share_dialog()

        self.pointing_device.move_to_object(share_button)
        self.assertThat(share_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        self.assertThat(share_menu.visible, Eventually(Equals(True)))

    def test_nav_bar_edit_button(self):
        """Clicking the edit button must show the edit dialog."""
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button()
        edit_dialog = self.photo_viewer.get_photo_edit_dialog()

        self.pointing_device.move_to_object(edit_button)
        self.assertThat(edit_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        self.assertThat(edit_dialog.visible, Eventually(Equals(True)))

    def test_double_click_zoom(self):
        """Double clicking an opened photo must zoom it."""
        opened_photo = self.photo_viewer.get_photo_component()

        self.pointing_device.move_to_object(opened_photo)
        self.pointing_device.click()
        self.pointing_device.click()

        self.assertThat(opened_photo.fullyZoomed, Eventually(Equals(True)))

        self.pointing_device.click()
        self.pointing_device.click()

        self.assertThat(opened_photo.fullyUnzoomed, Eventually(Equals(True)))

    def test_photo_crop_box_shows(self):
        """Clicking the crop item in the edit dialog must show crop interactor."""
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button()
        crop_item = self.photo_viewer.get_crop_menu_item()
        crop_box = self.photo_viewer.get_crop_interactor()

        self.pointing_device.move_to_object(edit_button)
        self.pointing_device.click()

        self.pointing_device.move_to_object(crop_item)
        self.pointing_device.click()

        self.assertThat(crop_box.visible, Eventually(Equals(True)))


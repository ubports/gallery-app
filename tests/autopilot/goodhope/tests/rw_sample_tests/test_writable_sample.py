# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photo Editor of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from goodhope.tests.rw_sample_tests import GoodhopeTestCase

from os.path import exists
import os

from time import sleep


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

    def click_edit_button(self):
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button()

        self.pointing_device.move_to_object(edit_button)
        self.pointing_device.click()

    def click_crop_item(self):
        crop_item = self.photo_viewer.get_crop_menu_item()

        self.pointing_device.move_to_object(crop_item)
        self.pointing_device.click()

    def test_photo_delete_works(self):
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

        self.assertThat(lambda: exists("/var/crash/sample.jpg"), Eventually(Equals(False)))

        #Up until the last line above we are testing if the file got delete, now here we
        #are re-copying the sample.jpg file because it seems the addCleanup method in __init__
        #tries to remove the sample.jpg but if it does not find it the test fails. So this
        #"hack" saves us from that. --om26er
        if exists("/usr/lib/python2.7/dist-packages/goodhope/data/sample.jpg"):
            os.system("cp /usr/lib/python2.7/dist-packages/goodhope/data/sample.jpg /var/crash/")
        else:
            os.system("cp goodhope/data/sample.jpg /var/crash/")

        self.assertThat(lambda: exists("/var/crash/sample.jpg"), Eventually(Equals(True)))

    def test_photo_cropping_works(self):
        """Cropping a photo must crop it."""
        self.click_edit_button()
        crop_box = self.photo_viewer.get_crop_interactor()

        self.click_crop_item()

        self.assertThat(crop_box.state, Eventually(Equals("shown")))

        sample_location = os.path.expanduser("/var/crash/sample.jpg")
        old_file_size = os.path.getsize(sample_location)

        crop_corner = self.photo_viewer.get_top_left_crop_corner()

        self.pointing_device.move_to_object(crop_corner)

        x, y, h, w = crop_corner.globalRect

        self.pointing_device.press()
        self.pointing_device.move(x + (w/2 + 400), y + (h/2 +300))
        self.pointing_device.release()

        sleep(1)

        crop_icon = self.photo_viewer.get_crop_overlays_crop_icon()
        self.pointing_device.move_to_object(crop_icon)
        self.pointing_device.click()

        new_file_size = os.path.getsize(sample_location)
        self.assertThat(lambda: old_file_size > new_file_size, Eventually(Equals(True)))

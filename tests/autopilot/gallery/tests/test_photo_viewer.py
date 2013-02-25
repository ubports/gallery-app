# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photo editor of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from gallery.tests import GalleryTestCase

from os.path import exists, expanduser
import os
import shutil

from time import sleep

"""
Class for common functionality of the phot viewing and photo editing
"""
class TestPhotoViewerBase(GalleryTestCase):
    def setUp(self):
        super(TestPhotoViewerBase, self).setUp()
        self.assertThat(self.photo_viewer.get_qml_view().visible, Eventually(Equals(True)))

        self.click_first_photo()
        photo_viewer_loader = self.photo_viewer.get_main_photo_viewer_loader()
        self.assertThat(photo_viewer_loader.loaded, Eventually(Equals(True)))

        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

        self.reveal_tool_bar()

    def click_first_photo(self):
        single_photo = self.photo_viewer.get_first_image_in_photo_viewer()
        self.click_item(single_photo)


class TestPhotoViewer(TestPhotoViewerBase):

    def setUp(self):
        super(TestPhotoViewer, self).setUp()

    def test_nav_bar_back_button(self):
        """Clicking the back button must close the photo."""
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        back_button = self.photo_viewer.get_cancel_icon()
        self.click_item(back_button)

        self.assertThat(photo_viewer.visible, Eventually(Equals(False)))

    def test_photo_delete_works(self):
        """Clicking the trash button must show the delete dialog."""
        tool_bar = self.photo_viewer.get_tool_bar()
        trash_button = self.photo_viewer.get_delete_icon(tool_bar)

        self.pointing_device.move_to_object(trash_button)
        self.assertThat(trash_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        delete_dialog = self.photo_viewer.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        cancel_item = self.photo_viewer.get_delete_popover_cancel_item()
        self.click_item(cancel_item)

        self.assertThat(delete_dialog.visible, Eventually(Equals(False)))

        self.reveal_tool_bar()

        self.pointing_device.move_to_object(trash_button)
        self.assertThat(trash_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        delete_item = self.photo_viewer.get_delete_popover_delete_item()
        self.click_item(delete_item)

        self.assertThat(lambda: exists(self.sample_file), Eventually(Equals(False)))

        #Up until the last line above we are testing if the file got delete, now here we
        #are re-copying the sample.jpg file because it seems the addCleanup method in __init__
        #tries to remove the sample.jpg but if it does not find it the test fails. So this
        #"hack" saves us from that. --om26er
        if os.path.realpath(__file__).startswith("/usr/"):
            shutil.copy(self.installed_sample_file, self.sample_dir)
        else:
            shutil.copy(self.local_sample_file, self.sample_dir)

        self.assertThat(lambda: exists(self.sample_file), Eventually(Equals(True)))

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
        tool_bar = self.photo_viewer.get_tool_bar()
        share_button = self.photo_viewer.get_viewer_chrome_share_button(tool_bar)
        share_menu = self.photo_viewer.get_share_dialog()

        self.pointing_device.move_to_object(share_button)
        self.assertThat(share_button.hovered, Eventually(Equals(True)))
        self.pointing_device.click()

        self.assertThat(share_menu.visible, Eventually(Equals(True)))

    def test_nav_bar_edit_button(self):
        """Clicking the edit button must show the edit dialog."""
        tool_bar = self.photo_viewer.get_tool_bar()
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button(tool_bar)
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


class TestPhotoEditor(TestPhotoViewerBase):

    def setUp(self):
        super(TestPhotoEditor, self).setUp()
        self.click_edit_button()

    def click_edit_button(self):
        tool_bar = self.photo_viewer.get_tool_bar()
        edit_button = self.photo_viewer.get_viewer_chrome_toolbar_edit_button(tool_bar)
        self.click_item(edit_button)

    def click_rotate_item(self):
        rotate_item = self.photo_viewer.get_rotate_menu_item()
        self.click_item(rotate_item)

    def click_crop_item(self):
        crop_item = self.photo_viewer.get_crop_menu_item()
        self.click_item(crop_item)

    def click_undo_item(self):
        undo_item = self.photo_viewer.get_undo_menu_item()
        self.click_item(undo_item)

    def click_redo_item(self):
        redo_item = self.photo_viewer.get_redo_menu_item()
        self.click_item(redo_item)

    def click_revert_item(self):
        revert_item = self.photo_viewer.get_revert_menu_item()
        self.click_item(revert_item)

    def test_photo_editor_crop(self):
        """Cropping a photo must crop it."""
        crop_box = self.photo_viewer.get_crop_interactor()
        item_width = crop_box.width
        item_height = crop_box.height

        self.click_crop_item()

        self.assertThat(crop_box.state, Eventually(Equals("shown")))

        old_file_size = os.path.getsize(self.sample_file)

        crop_corner = self.photo_viewer.get_top_left_crop_corner()

        self.pointing_device.move_to_object(crop_corner)

        x, y, h, w = crop_corner.globalRect

        self.pointing_device.press()
        self.pointing_device.move(x + (w/2 + item_width/2), y + (h/2 +item_height/2))
        self.pointing_device.release()

        sleep(1)

        crop_button = self.photo_viewer.get_crop_overlays_crop_icon()
        self.click_item(crop_button)

        sleep(0.5)
        new_file_size = os.path.getsize(self.sample_file)
        self.assertThat(old_file_size > new_file_size, Equals(True))

        # give the gallery the time to fully save the photo, and rebuild the thumbnails
        sleep(1)

    def test_photo_editor_rotate(self):
        """Makes sure that the photo editor inside the photo viewer works using the rotate function"""
        opened_photo = self.photo_viewer.get_opened_photo()
        item_height = opened_photo.height

        is_landscape = opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape, Equals(True))

        self.click_rotate_item()

        self.assertThat(opened_photo.paintedHeight, Eventually(Equals(item_height)))
        is_landscape = opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape, Equals(False))

        self.reveal_tool_bar()
        self.click_edit_button()
        self.click_undo_item()

        self.assertThat(opened_photo.paintedHeight, Eventually(NotEquals(item_height)))
        is_landscape = opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape, Equals(True))

        self.reveal_tool_bar()
        self.click_edit_button()
        self.click_redo_item()

        self.assertThat(opened_photo.paintedHeight, Eventually(Equals(item_height)))
        is_landscape = opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape, Equals(False))

        self.reveal_tool_bar()
        self.click_edit_button()
        self.click_rotate_item()
        self.reveal_tool_bar()
        self.click_edit_button()
        self.click_revert_item()

        self.assertThat(opened_photo.paintedHeight, Eventually(NotEquals(item_height)))
        is_landscape = opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape, Equals(True))

        # give the gallery the time to fully save the photo, and rebuild the thumbnails
        sleep(1)

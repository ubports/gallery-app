# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photos view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, GreaterThan
from autopilot.matchers import Eventually

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.photos_view import PhotosView

from os.path import exists
from time import sleep


class TestPhotosView(GalleryTestCase):

    @property
    def photos_view(self):
        return PhotosView(self.app)

    def setUp(self):
        super(TestPhotosView, self).setUp()
        self.switch_to_photos_tab()

    def switch_to_photos_tab(self):
        tabs_bar = self.photos_view.get_tabs_bar()
        self.click_item(tabs_bar)

        photos_tab_button = self.photos_view.get_photos_tab_button()
        # Due to some timing issues sometimes mouse moves to the location a bit
        # earlier even though the tab item is not fully visible, hence the tab
        # does not activate.
        self.assertThat(photos_tab_button.opacity,
                        Eventually(GreaterThan(0.2)))
        self.click_item(photos_tab_button)

        self.ensure_tabs_dont_move()

    def ensure_tabs_dont_move(self):
        # FIXME find a (functional) way to test if the tabs still move
        sleep(1)

    def enable_select_mode(self):
        self.reveal_toolbar()
        self.click_select_icon()

    def click_select_icon(self):
        select_icon = self.photos_view.get_toolbar_select_button()
        self.click_item(select_icon)

    def click_first_photo(self):
        photo = self.photos_view.get_first_photo_in_photos_view()
        self.click_item(photo)

    def click_delete_action(self):
        trash_button = self.photos_view.get_toolbar_delete_button()
        self.click_item(trash_button)

    def test_open_photo(self):
        self.click_first_photo()
        photo_viewer = self.photos_view.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

    def test_select_button_cancel(self):
        """Clicking the cancel button after clicking the select button must
        hide the toolbar automatically."""
        self.enable_select_mode()

        cancel_icon = self.photos_view.get_toolbar_cancel_icon()
        self.click_item(cancel_icon)

        toolbar = self.photos_view.get_toolbar()
        self.assertThat(toolbar.active, Eventually(Equals(False)))

    def test_delete_a_photo(self):
        """Selecting a photo must make the delete button clickable."""
        number_of_photos = self.photos_view.number_of_photos()
        self.enable_select_mode()
        self.click_first_photo()
        self.click_delete_action()

        delete_dialog = self.photos_view.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        cancel_item = self.photos_view.get_delete_dialog_cancel_button()
        self.click_item(cancel_item)

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(True)))

        new_number_of_photos = self.photos_view.number_of_photos()
        self.assertThat(new_number_of_photos, Equals(number_of_photos))

        self.click_delete_action()

        delete_dialog = self.photos_view.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        delete_item = self.photos_view.get_delete_dialog_delete_button()
        self.click_item(delete_item)

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(False)))

        self.ui_update()
        new_number_of_photos = self.photos_view.number_of_photos()
        self.assertThat(new_number_of_photos, Equals(number_of_photos - 1))

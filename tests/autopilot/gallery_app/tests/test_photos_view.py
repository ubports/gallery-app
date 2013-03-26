# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photos view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals, GreaterThan
from autopilot.matchers import Eventually

from gallery_app.tests import GalleryTestCase

from os.path import exists
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
        self.assertThat(photos_tab_button.opacity, Eventually(GreaterThan(0.2)))

        photos_tab = self.photos_view.get_photos_tab()
        self.click_item(photos_tab)

    def enable_select_mode(self):
        self.reveal_tool_bar()
        self.click_select_icon()

    def click_select_icon(self):
        select_icon = self.photos_view.get_select_icon()
        self.click_item(select_icon)

    def click_first_photo(self):
        photo = self.photos_view.get_first_photo_in_photos_view()
        self.click_item(photo)

    def click_delete_action(self):
        trash_button = self.photos_view.get_delete_icon()
        self.click_item(trash_button)


    def test_open_photo(self):
        self.click_first_photo()
        photo_viewer = self.photos_view.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

    def test_select_button_cancel(self):
        """Clicking the cancel button after clicking the select button must
        hide the chromebar automatically."""
        self.enable_select_mode()

        cancel_icon = self.photos_view.get_cancel_icon()
        self.click_item(cancel_icon)

        toolbar = self.photos_view.get_tool_bar()
        self.assertThat(toolbar.active, Eventually(Equals(False)))

    def test_delete_a_photo(self):
        """Selecting a photo must make the delete button clickable."""
        number_of_photos = self.photos_view.number_of_photos_in_photos_view()
        self.enable_select_mode()
        self.click_first_photo()
        self.click_delete_action()

        delete_dialog = self.photos_view.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        cancel_item = self.photos_view.get_delete_dialog_cancel_button()
        self.click_item(cancel_item)

        self.assertThat(lambda: exists(self.sample_file), Eventually(Equals(True)))

        new_number_of_photos = self.photos_view.number_of_photos_in_photos_view()
        self.assertThat(new_number_of_photos, Equals(number_of_photos))

        self.click_delete_action()

        delete_dialog = self.photos_view.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))

        delete_item = self.photos_view.get_delete_dialog_delete_button()
        self.click_item(delete_item)

        self.assertThat(lambda: exists(self.sample_file), Eventually(Equals(False)))

        new_number_of_photos = self.photos_view.number_of_photos_in_photos_view()
        self.assertThat(new_number_of_photos, Equals(number_of_photos-1))

# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photos view of the gallery app."""

from testtools.matchers import Equals, Is
from testtools import skipUnless
from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.introspection.dbus import StateNotFoundError

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.photos_view import PhotosView

from time import sleep
from os import environ as env
import unittest


class TestPhotosView(GalleryTestCase):
    envDesktopMode = None

    @property
    def photos_view(self):
        return PhotosView(self.app)

    def setUp(self):
        self.ARGS = []
        self.envDesktopMode = env.get("DESKTOP_MODE")

        if model() == "Desktop":
            env["DESKTOP_MODE"] = "1"
        else:
            env["DESKTOP_MODE"] = "0"

        super(TestPhotosView, self).setUp()
        self.switch_to_photos_tab()

    def tearDown(self):
        if self.envDesktopMode:
            env["DESKTOP_MODE"] = self.envDesktopMode
        else:
            del env["DESKTOP_MODE"]

        super(TestPhotosView, self).tearDown()

    def switch_to_photos_tab(self):
        self.main_view.switch_to_tab("photosTab")
        self.ensure_tabs_dont_move()

    def click_first_photo(self):
        photo = self.photos_view.get_first_photo_in_photos_view()
        self.click_item(photo)

    def select_first_photo(self):
        photo = self.photos_view.get_first_photo_in_photos_view()
        checkbox = photo.select_single(objectName="selectionCheckbox")
        self.click_item(checkbox)

    def check_header_button_exist(self, button):
        header = self.main_view.get_header()
        buttonName = button + "_header_button"
        try:
            header.select_single(objectName=buttonName)
        except StateNotFoundError:
            return False
        return True

    def test_open_photo(self):
        self.click_first_photo()
        sleep(5)
        photo_viewer = self.photos_view.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

    def test_select_button_cancel(self):
        """Clicking the cancel button after clicking the select button must
        hide the toolbar automatically."""
        photos_overview = self.app.select_single("PhotosOverview")
        self.assertFalse(photos_overview.inSelectionMode)

        self.main_view.get_header().click_action_button("selectButton")
        self.assertTrue(photos_overview.inSelectionMode)

        self.main_view.get_header().click_custom_back_button()

        self.assertFalse(photos_overview.inSelectionMode)

        first_photo = self.photos_view.get_first_photo_in_photos_view()
        self.tap_item(first_photo)
        self.assertTrue(photos_overview.inSelectionMode)

    def test_delete_photo_dialog_appears(self):
        """Selecting a photo must make the delete button clickable."""
        self.main_view.get_header().click_action_button("selectButton")
        self.select_first_photo()
        self.main_view.get_header().click_action_button("deleteButton")

        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(True)))

        cancel_item = self.photos_view.get_delete_dialog_cancel_button()
        self.click_item(cancel_item)
        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(False)))

    def test_delete_a_photo(self):
        """Must be able to select a photo and use the dialog to delete it."""
        number_of_photos = self.photos_view.number_of_photos()
        self.main_view.get_header().click_action_button("selectButton")
        self.select_first_photo()
        self.main_view.get_header().click_action_button("deleteButton")

        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(True)))

        delete_item = self.photos_view.get_delete_dialog_delete_button()
        self.click_item(delete_item)
        self.assertThat(
            self.gallery_utils.delete_dialog_shown,
            Eventually(Is(False))
        )

        self.assertThat(lambda: self.photos_view.number_of_photos(),
                        Eventually(Equals(number_of_photos - 1)))

    @unittest.skip("Temporarily disable as it fails in some cases, "
                   "supposedly due to problems with the infrastructure")
    def test_save_state(self):
        self.switch_to_photos_tab()

        tabs = self.main_view.select_single("Tabs")
        tab = tabs.get_current_tab()
        self.assertThat(tab.objectName, Equals("photosTab"))
        index = tab.index

        self.ensure_app_has_quit()
        self.start_app()

        tabs = self.main_view.select_single("Tabs")
        tab = tabs.get_current_tab()
        self.assertThat(tabs.selectedTabIndex, Eventually(Equals(index)))
        self.assertThat(tab.objectName, Equals("photosTab"))

    @skipUnless(
        model() == 'Desktop',
        'Key based tests only make sense on Desktop'
    )
    def test_toggle_fullscreen(self):
        self.switch_to_photos_tab()
        view = self.main_view
        self.assertThat(view.fullScreen, Eventually(Equals(False)))
        self.keyboard.press_and_release('F11')
        self.assertThat(view.fullScreen, Eventually(Equals(True)))
        self.keyboard.press_and_release('F11')
        self.assertThat(view.fullScreen, Eventually(Equals(False)))
        self.keyboard.press_and_release('F11')
        self.assertThat(view.fullScreen, Eventually(Equals(True)))
        self.keyboard.press_and_release('Escape')
        self.assertThat(view.fullScreen, Eventually(Equals(False)))
        self.keyboard.press_and_release('Escape')
        self.assertThat(view.fullScreen, Eventually(Equals(False)))

    # Check if Camera Button is not visible at Desktop mode
    def test_camera_button_visible(self):
        cameraButtonVisible = self.check_header_button_exist("cameraButton")
        if model() == "Desktop":
            self.assertThat(cameraButtonVisible, Equals(False))
        else:
            self.assertThat(cameraButtonVisible, Equals(True))

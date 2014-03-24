# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests for the Gallery App"""

from __future__ import absolute_import

from testtools.matchers import Equals, Is, GreaterThan
from autopilot.matchers import Eventually
from autopilot.platform import model

from gallery_app.tests import GalleryTestCase
from gallery_app.emulators.events_view import EventsView

from os import environ as env
from os.path import exists
import shutil

class TestEventsView(GalleryTestCase):
    """Tests the main gallery features"""
    envDesktopMode = None

    @property
    def events_view(self):
        return EventsView(self.app)

    def setUp(self):
        self.ARGS = []

        self.envDesktopMode = env.get("DESKTOP_MODE")

        if model() == "Desktop":
            env["DESKTOP_MODE"] = "1"
        else:
            env["DESKTOP_MODE"] = "0"

        # This is needed to wait for the application to start.
        # In the testfarm, the application may take some time to show up.
        super(TestEventsView, self).setUp()
        self.main_view.switch_to_tab("eventsTab")
        """Wait for the data to be loaded and displayed"""
        self.assertThat(lambda: self.events_view.number_of_events(),
                        Eventually(GreaterThan(0)))

    def tearDown(self):
        if self.envDesktopMode:
            env["DESKTOP_MODE"] = self.envDesktopMode
        else:
            del env["DESKTOP_MODE"]

        super(TestEventsView, self).tearDown()

    def get_events_view(self):
        return self.app.wait_select_single("EventsOverview",
                                           objectName="organicEventView")

    def enable_select_mode(self):
        self.main_view.open_toolbar().click_button("selectButton")

    def click_first_photo(self):
        first_photo = self.events_view.get_first_image_in_event_view()
        self.click_item(first_photo)

    def assert_delete_dialog_visible(self):
        delete_dialog = self.gallery_utils.get_delete_dialog()

        self.assertThat(delete_dialog.opacity, Eventually(Equals(1)))

    def test_select_button_cancel(self):
        """Clicking the cancel button after clicking the select button must
           hide the toolbar automatically."""
        events_view = self.get_events_view()
        self.assertFalse(events_view.inSelectionMode)

        self.enable_select_mode()
        self.assertTrue(events_view.inSelectionMode)

        self.main_view.get_toolbar().click_custom_button("cancelButton")

        toolbar = self.main_view.get_toolbar()
        self.assertThat(toolbar.opened, Eventually(Equals(False)))
        self.assertFalse(events_view.inSelectionMode)

        first_photo = self.events_view.get_first_image_in_event_view()
        self.tap_item(first_photo)
        self.assertTrue(events_view.inSelectionMode)

    def test_delete_a_photo(self):
        """Selecting a photo must make the delete button clickable."""
        number_of_photos = self.events_view.number_of_photos_in_events()
        self.enable_select_mode()
        self.click_first_photo()
        self.main_view.open_toolbar().click_button("deleteButton")
        self.assert_delete_dialog_visible()

        cancel_item = self.gallery_utils.get_delete_dialog_cancel_button()
        self.click_item(cancel_item)

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(True)))

        new_number_of_photos = self.events_view.number_of_photos_in_events()
        self.assertThat(new_number_of_photos, Equals(number_of_photos))

        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(False)))

        self.main_view.open_toolbar().click_button("deleteButton")
        self.assert_delete_dialog_visible()

        delete_item = self.gallery_utils.get_delete_dialog_delete_button()
        self.click_item(delete_item)
        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(False)))

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(False)))

        self.ui_update()
        new_number_of_photos = self.events_view.number_of_photos_in_events()
        self.assertThat(new_number_of_photos, Equals(number_of_photos - 1))

    def test_adding_a_video(self):
        events_before = self.events_view.number_of_events()
        video_file = "video20130618_0002.mp4"
        shutil.copyfile(self.sample_dir+"/option01/"+video_file,
                        self.sample_destination_dir+"/"+video_file)
        video_file = "clip_0001.mkv"
        shutil.copyfile(self.sample_dir+"/option01/"+video_file,
                        self.sample_destination_dir+"/"+video_file)
        self.assertThat(
            lambda: self.events_view.number_of_events(),
            Eventually(Equals(events_before + 2)))

    # Check if Camera Button is not visible at Desktop mode
    def test_camera_button_visible(self):
        self.main_view.open_toolbar()
        toolbar = self.main_view.get_toolbar()
        cameraButton = toolbar.select_single("ActionItem", objectName="cameraButton")
        if model() == "Desktop":
            self.assertThat(cameraButton.visible, Equals(False))
        else:
            self.assertThat(cameraButton.visible, Equals(True))


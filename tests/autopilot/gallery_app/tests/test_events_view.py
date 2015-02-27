# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests for the Gallery App"""

from testtools.matchers import Equals, NotEquals, Is, GreaterThan
from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.introspection.dbus import StateNotFoundError

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
        return self.app.wait_select_single("EventsOverview")

    def enable_select_mode(self):
        self.main_view.get_header().click_action_button("selectButton")

    def check_header_button_exist(self, button):
        header = self.main_view.get_header()
        buttonName = button + "_header_button"
        try:
            header.select_single(objectName=buttonName)
        except StateNotFoundError:
            return False
        return True

    def test_select_button_cancel(self):
        """Clicking the cancel button after clicking the select button must
           hide the toolbar automatically."""
        events_view = self.get_events_view()
        self.assertFalse(events_view.inSelectionMode)

        self.enable_select_mode()
        self.assertTrue(events_view.inSelectionMode)

        self.main_view.get_header().click_custom_back_button()

        self.assertFalse(events_view.inSelectionMode)

    def test_delete_a_photo(self):
        """Selecting a photo must make the delete button clickable."""
        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(True)))

        self.enable_select_mode()
        self.events_view.select_photo(self.sample_file)
        self.main_view.get_header().click_action_button("deleteButton")
        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(True)))

        self.gallery_utils.click_delete_dialog_cancel_button()
        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(False)))

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(True)))

        self.main_view.get_header().click_action_button("deleteButton")
        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(True)))

        self.gallery_utils.click_delete_dialog_delete_button()
        self.assertThat(self.gallery_utils.delete_dialog_shown,
                        Eventually(Is(False)))

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(False)))

    def test_adding_a_video(self):
        if model() == "Desktop":
            before = self.events_view.get_event(0)
            video_file = "video.mp4"
            shutil.copyfile(self.sample_dir+"/option01/"+video_file,
                            self.sample_destination_dir+"/"+video_file)
            video_file = "video.mkv"
            shutil.copyfile(self.sample_dir+"/option01/"+video_file,
                            self.sample_destination_dir+"/"+video_file)
            after = self.events_view.get_event(0)
            self.assertThat(lambda: str(after),
                            Eventually(NotEquals(str(before))))
            self.assertThat(
                lambda: self.events_view.number_of_photos_in_events(),
                Eventually(Equals(4)))

    # Check if Camera Button is not visible at Desktop mode
    def test_camera_button_visible(self):
        cameraButtonVisible = self.check_header_button_exist("cameraButton")
        if model() == "Desktop":
            self.assertThat(cameraButtonVisible, Equals(False))
        else:
            self.assertThat(cameraButtonVisible, Equals(True))

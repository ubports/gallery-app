# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests for the Gallery App"""

from __future__ import absolute_import

from testtools.matchers import Equals, Is
from autopilot.matchers import Eventually

from gallery_app.tests import GalleryTestCase

from os.path import exists


class TestEventsView(GalleryTestCase):
    """Tests the main gallery features"""

    def setUp(self):
        self.ARGS = []
        # This is needed to wait for the application to start.
        # In the testfarm, the application may take some time to show up.
        super(TestEventsView, self).setUp()
        self.main_view.switch_to_tab("eventsTab")

    def tearDown(self):
        super(TestEventsView, self).tearDown()

    def get_events_view(self):
        return self.app.wait_select_single("EventsOverview",
                                           objectName="organicEventView")

    def enable_select_mode(self):
        self.main_view.open_toolbar().click_button("selectButton")

    def click_first_photo(self):
        first_photo = self.gallery_utils.get_first_image_in_event_view()
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

        first_photo = self.gallery_utils.get_first_image_in_event_view()
        self.tap_item(first_photo)
        self.assertTrue(events_view.inSelectionMode)

    def test_delete_a_photo(self):
        """Selecting a photo must make the delete button clickable."""
        number_of_photos = self.gallery_utils.number_of_photos_in_events()
        self.enable_select_mode()
        self.click_first_photo()
        self.main_view.open_toolbar().click_button("deleteButton")
        self.assert_delete_dialog_visible()

        cancel_item = self.gallery_utils.get_delete_dialog_cancel_button()
        self.click_item(cancel_item)

        self.assertThat(lambda: exists(self.sample_file),
                        Eventually(Equals(True)))

        new_number_of_photos = self.gallery_utils.number_of_photos_in_events()
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
        new_number_of_photos = self.gallery_utils.number_of_photos_in_events()
        self.assertThat(new_number_of_photos, Equals(number_of_photos - 1))

    def test_adding_a_video(self):
        self.add_video_sample()

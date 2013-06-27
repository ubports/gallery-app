# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photos view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, GreaterThan
from autopilot.matchers import Eventually

from gallery_app.emulators.picker_mode import PickerMode
from gallery_app.tests import GalleryTestCase

from os.path import exists
from time import sleep


class TestPickerMode(GalleryTestCase):

    @property
    def picker_mode(self):
        return PickerMode(self.app)

    def setUp(self):
        self.ARGS.append("--pick-photo")
        super(TestPickerMode, self).setUp()
        
    def select_first_event_media(self):
        first_media = self.picker_mode.first_media_in_events_view()
        self.click_item(first_media)
        
    def select_first_grid_media(self):
        first_media = self.picker_mode.first_media_in_events_view()
        self.click_item(first_media)
        
    def switch_to_grid_view(self):
        tabs_bar = self.picker_mode.get_tabs_bar()
        self.click_item(tabs_bar)
        photos_tab_button = self.picker_mode.get_photos_tab_button()
        # Due to some timing issues sometimes mouse moves to the location a bit
        # earlier even though the tab item is not fully visible, hence the tab
        # does not activate.
        self.assertThat(photos_tab_button.opacity,
                        Eventually(GreaterThan(0.2)))
        self.click_item(photos_tab_button)
        self.ensure_tabs_dont_move()

    def test_pick_first_photo(self):
        """Check if the button enabled state follows the selection"""
        pick_button = self.picker_mode.pick_button()
        self.assertThat(pick_button.enabled, Eventually(Equals(False)))
        first_events_media = self.picker_mode.first_media_in_events_view()
        self.assertThat(first_events_media.isSelected, Eventually(Equals(False)))
        
        self.select_first_event_media()
        
        pick_button = self.picker_mode.pick_button()
        self.assertThat(pick_button.enabled, Eventually(Equals(True)))
        self.assertThat(first_events_media.isSelected, Eventually(Equals(True)))
        
        self.select_first_event_media()
        
        pick_button = self.picker_mode.pick_button()
        self.assertThat(pick_button.enabled, Eventually(Equals(False)))
        self.assertThat(first_events_media.isSelected, Eventually(Equals(False)))

    def test_selection_synchronisation(self):
        """Checks if the selection is the same for both views"""
        first_events_media = self.picker_mode.first_media_in_events_view()
        self.assertThat(first_events_media.isSelected, Eventually(Equals(False)))
        
        self.select_first_event_media()
        self.assertThat(first_events_media.isSelected, Eventually(Equals(True)))
        
        self.switch_to_grid_view()

        first_grid_media = self.picker_mode.first_media_in_grid_view()
        self.assertThat(first_grid_media.isSelected, Eventually(Equals(True)))


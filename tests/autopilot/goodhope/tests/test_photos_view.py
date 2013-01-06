# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photos view of the gallery app."""

from __future__ import absolute_import

from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from goodhope.tests import GoodhopeTestCase


class TestPhotosView(GoodhopeTestCase):

    def setUp(self):
        super(TestPhotosView, self).setUp()
        self.assertThat(self.main_window.get_qml_view().visible, Eventually(Equals(True)))

        tabs_bar = self.photos_view.get_tabs_bar()
        self.mouse.move_to_object(tabs_bar)
        self.mouse.click()

        photos_tab_button = self.app.select_single("AbstractButton", buttonIndex=3)

        #Due to some timing issues sometimes mouse moves to the location a bit earlier
        #even though the tab item is not fully visible, hence the tab does not activate.
        self.assertTrue(photos_tab_button.opacity, Eventually(Equals("=<0.2")))

        photos_tab = self.photos_view.get_photos_tab()
        self.mouse.move_to_object(photos_tab)
        self.mouse.click()

        photos_view = self.photos_view.get_photos_view()

        self.assertThat(photos_view.focus, Eventually(Equals(True)))

    def test_this(self):
        pass

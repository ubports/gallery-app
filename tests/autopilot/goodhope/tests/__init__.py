# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""goodhope autopilot tests."""

from os import remove
import os.path

from autopilot.introspection.qt import QtIntrospectionTestMixin
from autopilot.testcase import AutopilotTestCase

from goodhope.emulators.main_window import MainWindow

class GoodhopeTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for goodhope tests."""

    def setUp(self):
        super(GoodhopeTestCase, self).setUp()
        os.system("cp goodhope/data/sample.jpg ~/Pictures/")

        sample_location = os.path.expanduser("~/Pictures/sample.jpg")

        self.assertTrue(os.path.exists(sample_location))
        self.addCleanup(os.remove, sample_location)

        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            self.launch_test_installed()
        else:
            self.launch_test_local()

    def launch_test_local(self):
        self.app = self.launch_test_application(
            "../../gallery"
            )

    def launch_test_installed(self):
        self.app = self.launch_test_application(
           "gallery",
           )

    @property
    def main_window(self):
        return MainWindow(self.app)


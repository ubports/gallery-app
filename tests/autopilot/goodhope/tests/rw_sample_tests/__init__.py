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

from goodhope.emulators.events_view import EventsView
from goodhope.emulators.photo_viewer import PhotoViewer


class GoodhopeTestCase(AutopilotTestCase, QtIntrospectionTestMixin):

    """A common test case class that provides several useful methods for goodhope tests."""

    def setUp(self):
        super(GoodhopeTestCase, self).setUp()
        # Lets assume we are installed system wide if this file is somewhere in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            self.launch_test_installed()
            os.system("cp /usr/lib/python2.7/dist-packages/goodhope/data/sample.jpg /var/crash")
        else:
            self.launch_test_local()
            os.system("cp goodhope/data/sample.jpg /var/crash")

        sample_location = os.path.expanduser("/var/crash/sample.jpg")

        self.addCleanup(os.remove, sample_location)

    def launch_test_local(self):
        self.app = self.launch_test_application(
            "../../src/gallery", "/var/crash/"
            )

    def launch_test_installed(self):
        self.app = self.launch_test_application(
           "gallery", "/var/crash/"
           )

    @property
    def events_view(self):
        return EventsView(self.app)

    @property
    def photo_viewer(self):
        return PhotoViewer(self.app)

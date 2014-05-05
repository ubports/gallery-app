# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators


class MediaViewer(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """A class that makes it easy to interact with the media viewer"""

    def get_edit_spinner(self):
        return self.select_single(
            "ActivityIndicator",
            objectName="busySpinner"
        )

    def ensure_spinner_not_running(self):
        """Wait for spinner to stop running"""
        spinner = self.get_edit_spinner()
        spinner.running.wait_for(False)

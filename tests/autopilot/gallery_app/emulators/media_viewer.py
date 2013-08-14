# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators


class MediaViewer(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """An emulator class that makes it easy to interact with the media viewer"""

    def __init__(self, *args):
        super(MediaViewer, self).__init__(*args)

    def get_edit_spinner(self):
        return self.select_single("ActivityIndicator")

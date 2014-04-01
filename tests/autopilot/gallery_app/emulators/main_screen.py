# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013, 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators
from gallery_app.emulators import toolbar


class MainScreen(toolkit_emulators.MainView):
    """An emulator class that makes it easy to interact with the gallery app"""

    def get_toolbar(self):
        """Return the Toolbar emulator of the MainView.

        Overriden because the gallery app has custom buttons.

        """
        return self.select_single(toolbar.Toolbar)

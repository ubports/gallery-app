# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013, 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import ubuntuuitoolkit


class MainScreen(ubuntuuitoolkit.MainView):
    """An emulator class that makes it easy to interact with the gallery app"""

    def get_header(self):
        """Return the Header emulator of the MainView."""
        return self.select_single(AppHeader, objectName='MainView_Header')


class AppHeader(ubuntuuitoolkit.Header):
    """Header Autopilot helper.

    We override this helper because on the gallery the gesture to show the
    header when it's hidden it's not the default.

    """

    # XXX We are overriding internal methods that may change on the toolkit.
    # The helper on the toolkit needs a public method that will be safe to
    # override. Reported as bug http://pad.lv/1363591 --elopio - 2014-31-08

    def _is_visible(self):
        return self.visible

    def _show(self):
        self.pointing_device.click_object(self._get_top_container())
        self.visible.wait_for(True)

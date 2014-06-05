# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013, 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.introspection.dbus import StateNotFoundError
import logging
from ubuntuuitoolkit import emulators as toolkit_emulators

logger = logging.getLogger(__name__)


class Toolbar(toolkit_emulators.Toolbar):
    """An emulator class that makes it easy to interact with the tool bar"""

    def click_button(self, object_name):
        """Click a button of the toolbar.

        The toolbar should be opened before clicking the button, or an
        exception will be raised. If the toolbar is closed for some reason
        (e.g., timer finishes) after moving the mouse cursor and before
        clicking the button, it is re-opened automatically by this function.

        Overriden from the toolkit because of bug http://pad.lv/1302706
        TODO remove this method once that bug is fixed on the toolkit.
        --elopio - 2014-04-04

        :parameter object_name: The QML objectName property of the button.
        :raise ToolkitEmulatorException: If there is no button with that object
            name.

        """
        # ensure the toolbar is open
        if not self.opened:
            raise toolkit_emulators.ToolkitEmulatorException(
                'Toolbar must be opened before calling click_button().')
        try:
            button = self._get_button(object_name)
        except StateNotFoundError:
            raise toolkit_emulators.ToolkitEmulatorException(
                'Button with objectName "{0}" not found.'.format(object_name))
        self.pointing_device.move_to_object(button)
        # ensure the toolbar is still open (may have closed due to timeout)
        self.open()
        # click the button
        self.pointing_device.click_object(button)

    def click_custom_button(self, object_name):
        try:
            button = self.select_single('Button', objectName=object_name)
            self.pointing_device.click_object(button)
        except StateNotFoundError:
            logger.error(
                'Button with objectName "{0}" not found.'.format(object_name)
            )
            raise

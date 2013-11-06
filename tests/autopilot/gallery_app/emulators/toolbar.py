# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
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

    def click_custom_button(self, object_name):
        try:
            button = self.select_single('Button', objectName=object_name)
            self.pointing_device.click_object(button)
        except StateNotFoundError:
            logger.error(
                'Button with objectName "{0}" not found.'.format(object_name)
            )
            raise

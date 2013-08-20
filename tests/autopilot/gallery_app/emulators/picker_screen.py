# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators


class PickerScreen(toolkit_emulators.MainView):

    def pick_button(self):
        return self.select_single("Button", objectName="pickButton")

    def get_photos_tab_button(self):
        """Returns the photos tab."""
        return self.select_single("AbstractButton", buttonIndex=3)

    def events_view(self):
        return self.select_single("OrganicView", objectName="eventSelectView")

    def first_media_in_events_view(self):
        events_view = self.events_view()
        return events_view.select_many("OrganicItemInteraction",
                                       objectName="eventsViewPhoto")[0]

    def grid_view(self):
        return self.select_single("MediaGrid")

    def first_media_in_grid_view(self):
        grid_view = self.grid_view()
        return grid_view.select_many("OrganicItemInteraction")[0]


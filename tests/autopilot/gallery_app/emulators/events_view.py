# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_utils import GalleryUtils

class EventsView(GalleryUtils):

    def __init__(self, app):
        self.app = app

    def get_first_event(self):
        """Returns the first event in the event view"""
        return self.app.select_single("OrganicMediaList",
                                      objectName="organicEventItem0")

    def number_of_events(self):
        """Returns the number of events in the model behind the event view"""
        return self.app.select_single("EventsOverview")._eventCount

    def number_of_photos_in_events(self):
        """Returns the number of events"""
        photo_delegates = self.app.select_many("QQuickItem",
                                               objectName="eventPhoto")
        return len(photo_delegates)

    def get_first_image_in_event_view(self):
        """Returns the first photo of the gallery."""
        event = self.get_first_event()
        return event.select_many("OrganicItemInteraction",
                                 objectName='eventsViewPhoto')[1]

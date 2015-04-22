# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from ubuntuuitoolkit import emulators as toolkit_emulators
from gallery_app.emulators.gallery_utils import GalleryAppException


class PickerScreen(toolkit_emulators.MainView):

    def pick_button(self):
        return self.select_single(objectName="pickButton_header_button")

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

    def go_to_photos(self):
        """
        Switch to the photos tab on picker screen
        Return the Page object representing the photos
        """
        self.switch_to_tab('photosTab')
        return self.select_single(Page11, objectName='photosPage')

    def click_pick_button(self):
        """Click on the pick button"""
        self.pointing_device.click_object(self.pick_button())


class Page11(PickerScreen):
    """Class to represent photos page view from picker screen"""

    def _get_named_photo_element(self, photo_name):
        """
        Return the ShapeItem container object for the named photo.
        This object can be clicked to enable the photo to be selected.
        """
        elements = self.grid_view().select_many('QQuickImage')
        for element in elements:
            element.status.wait_for(1)
            src = element.source.split('?')[0]
            if str(src).endswith(photo_name):
                return element.get_parent()
        raise GalleryAppException(
            'Photo with image name {} could not be found'.format(photo_name))

    def select_named_photo(self, photo_name):
        """Select the named photo from the picker view."""
        photo_element = self._get_named_photo_element(photo_name)
        checkbox = photo_element.select_single(objectName="selectionCheckbox")
        self.pointing_device.click_object(checkbox)

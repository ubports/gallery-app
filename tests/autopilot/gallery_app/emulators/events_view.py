# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_app.emulators.gallery_utils import(
    GalleryAppException,
    GalleryUtils
)


class EventsView(GalleryUtils):

    def __init__(self, app):
        super(EventsView, self).__init__(self)
        self.app = app

    def get_event(self, event_number=0):
        """Return an event in the event view based on index number

        :param event_number: the index number of the organicEventItem to get
        """
        return self.app.select_single(
            'OrganicMediaList',
            objectName='organicEventItem{}'.format(int(event_number))
        )

    def number_of_events(self):
        """Return the number of events in the model behind the event view"""
        return self.app.select_single('EventsOverview')._eventCount

    def number_of_photos_in_events(self):
        """Return the number of photos in events"""

        overview = self.app.select_single('EventsOverview')
        photo_delegates = overview.select_many(
            "QQuickItem",
            objectName="eventPhoto"
        )
        return len(photo_delegates)

    def number_of_photos_in_event(self, event):
        """Return the number of photo delgated in an event"""
        photo_delegates = event.select_many(objectName='eventPhoto')
        return len(photo_delegates)

    def _get_image_in_event_view(self, image_name):
        """Return the photo of the gallery based on image name.

        :param image_name: the name of the photo in the event to return"""
        events = self.app.select_many('OrganicMediaList')
        for event in events:
            photos = event.select_many(
                'QQuickItem',
                objectName='eventPhoto'
            )
            for photo in photos:
                images = photo.select_many('QQuickImage')
                for image in images:
                    image.status.wait_for(1)
                    src = image.source.split('?')[0]
                    if str(src).endswith(image_name):
                        return image
        raise GalleryAppException(
            'Photo with image name {} could not be found'.format(image_name))

    def _get_item_in_event_view(self, image_name, event_index_num=0):
        """Return the item of the gallery based on image name.

        :param image_name: the name of the photo in the event to return"""
        event = self.get_event(event_index_num)
        photos = event.select_many(
            'QQuickItem',
            objectName='eventPhoto'
        )
        for photo in photos:
            images = photo.select_many('QQuickImage')
            for image in images:
                src = image.source.split('?')[0]
                if str(src).endswith(image_name):
                    return photo
        raise GalleryAppException(
            'Photo with image name {} could not be found'.format(image_name))

    def click_photo(self, photo_name):
        """Click photo with name and event

        :param photo_name: name of file to click
        """
        photo = self._get_image_in_event_view(photo_name)
        self.pointing_device.click_object(photo)

    def select_photo(self, photo_name, event_index_num=0):
        """Select photo with name and event

        :param photo_name: name of file to click
        :param event_index_num: index of event to click
        """
        photo = self._get_item_in_event_view(photo_name, event_index_num)
        checkbox = photo.select_single(objectName="selectionCheckbox")
        self.pointing_device.click_object(checkbox)

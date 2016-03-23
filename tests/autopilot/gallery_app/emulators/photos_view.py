# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gallery_app.emulators.gallery_utils import GalleryUtils


class PhotosView(GalleryUtils):

    def __init__(self, app):
        self.app = app

    def get_photo_in_photos_view_by_index(self, index):
        """Returns the photo with index in the photos view."""
        return self.select_many_retry(
            "QQuickItem",
            objectName="allPotosGridPhoto")[index]

    def get_first_photo_in_photos_view(self):
        """Returns the very first photo in the photos view."""
        return self.get_photo_in_photos_view_by_index(0)

    def number_of_photos(self):
        """Returns the number of events"""
        photo_delegates = self.app.select_many("QQuickItem",
                                               objectName="allPotosGridPhoto")
        return len(photo_delegates)

    def get_share_peer_picker(self):
        """Returns the photo viewer share picker."""
        return self.app.wait_select_single(objectName="sharePickerPhotos",
                                           visible=True)

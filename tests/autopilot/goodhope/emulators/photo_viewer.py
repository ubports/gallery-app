# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


class PhotoViewer(object):

    def __init__(self, app):
        self.app = app

    def get_qml_view(self):
        """Get the main QML view"""
        return self.app.select_single("QQuickView")

    def get_first_image_in_photo_viewer(self):
        return self.app.select_many("GalleryPhotoComponent", ownerName='OrganicTrayView')[0]

    def get_photo_viewer(self):
        return self.app.select_single("PhotoViewer", objectName='photoViewer', currentIndex=0)

    def get_photo_viewer_chrome(self):
        pv = self.get_photo_viewer()
        return pv.select_single("ViewerChrome", fadeDuration=175)

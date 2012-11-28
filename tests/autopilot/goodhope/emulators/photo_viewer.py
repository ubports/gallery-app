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

    def get_overview(self):
        return self.app.select_single("Overview", objectName='overview')

    def get_first_image_in_photo_viewer(self):
        return self.app.select_many("GalleryPhotoComponent", ownerName='OrganicTrayView')[0]

    def get_main_photo_viewer(self):
        ov_ppv = self.get_overview().get_children_by_type("PopupPhotoViewer")[0]
        return ov_ppv.get_children_by_type("GalleryPhotoViewer")[0].get_children_by_type("PhotoViewer")[0]

    def get_photo_viewer_chrome(self):
        pv = self.get_photo_viewer()
        return pv.select_single("ViewerChrome", fadeDuration=175)

    def get_photo_viewer(self):
        return self.app.select_single("PhotoViewer", objectName='photoViewer', currentIndex=0)

    def get_viewer_chrome_back_button(self):
        ov = self.get_overview().get_children_by_type("ViewerChrome")[0]
        gst = ov.get_children_by_type("GalleryStandardNavbar")[0]
        qqr = gst.get_children_by_type("QQuickRow")[0]
        return qqr.get_children_by_type("StandardToolbarIconButton")[0]

    def get_viewer_chrome_trash_button(self):
        mpv = self.get_main_photo_viewer()
        gst = mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("GalleryStandardToolbar")[0]
        qqr = gst.get_children_by_type("QQuickRow")[1]
        return qqr.get_children_by_type("StandardToolbarIconButton")[0]

    def get_delete_dialog(self):
        mpv = self.get_main_photo_viewer()
        vc_dd = mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("DeleteDialog")[0]



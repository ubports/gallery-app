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
        """Returns the first photo of the gallery."""
        return self.app.select_many("GalleryPhotoComponent", ownerName='OrganicTrayView')[0]

    def get_main_photo_viewer(self):
        """Returns the PhotoViewer."""
        ov_ppv = self.get_overview().get_children_by_type("PopupPhotoViewer")[0]
        return ov_ppv.get_children_by_type("GalleryPhotoViewer")[0].get_children_by_type("PhotoViewer")[0]

    def get_crop_interactor(self):
        """Returns the crop interactor."""
        ov_ppv = self.get_overview().get_children_by_type("PopupPhotoViewer")[0]
        return ov_ppv.get_children_by_type("GalleryPhotoViewer")[0].get_children_by_type("CropInteractor")[0]

    def get_viewer_chrome_toolbar_buttons(self):
        """Returns the toolbar buttons."""
        mpv = self.get_main_photo_viewer()
        gst = mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("GalleryStandardToolbar")[0]
        return gst.get_children_by_type("QQuickRow")[1]

    def get_viewer_chrome_toolbar_edit_button(self):
        """Returns the edit button of the photo viewer toolbar."""
        mpv = self.get_main_photo_viewer()
        gst = mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("GalleryStandardToolbar")[0]
        return gst.get_children_by_type("QQuickRow")[0].get_children_by_type("ToolbarIconButton")[1]

    def get_photo_viewer_chrome(self):
        pv = self.get_photo_viewer()
        return pv.select_single("ViewerChrome", fadeDuration=175)

    def get_photo_viewer(self):
        return self.app.select_single("PhotoViewer", objectName='photoViewer', currentIndex=0)

    def get_viewer_chrome_back_button(self):
        """Returns the photo viewer back button."""
        ov = self.get_overview().get_children_by_type("ViewerChrome")[0]
        gst = ov.get_children_by_type("GalleryStandardNavbar")[0]
        qqr = gst.get_children_by_type("QQuickRow")[0]
        return qqr.get_children_by_type("StandardToolbarIconButton")[0]

    def get_viewer_chrome_trash_button(self):
        """Returns the photo viewer trash button."""
        qqr = self.get_viewer_chrome_toolbar_buttons()
        return qqr.get_children_by_type("StandardToolbarIconButton")[0]

    def get_viewer_chrome_album_button(self):
        """Returns the photo viewer album button."""
        qqr = self.get_viewer_chrome_toolbar_buttons()
        return qqr.get_children_by_type("StandardToolbarIconButton")[1]

    def get_viewer_chrome_share_button(self):
        """Returns the photo viewer share button."""
        qqr = self.get_viewer_chrome_toolbar_buttons()
        return qqr.get_children_by_type("StandardToolbarIconButton")[2]

    def get_viewer_chrome_more_button(self):
        """Returns the photo viewer 'more items' button."""
        qqr = self.get_viewer_chrome_toolbar_buttons()
        return qqr.get_children_by_type("StandardToolbarIconButton")[3]

    def get_delete_dialog(self):
        """Returns the photo viewer delete dialog."""
        mpv = self.get_main_photo_viewer()
        return mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("DeleteDialog")[0]

    def get_popup_album_picker(self):
        """Returns the photo viewer album pickers."""
        mpv = self.get_main_photo_viewer()
        return mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("PopupAlbumPicker")[0]

    def get_share_dialog(self):
        """Returns the photo viewer share dialog."""
        mpv = self.get_main_photo_viewer()
        return mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("PopupMenu")[0]

    def get_more_dialog(self):
        """Returns the photo viewer more items dialog."""
        mpv = self.get_main_photo_viewer()
        return mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("PopupMenu")[1]

    def get_photo_edit_dialog(self):
        """Returns the photo edit dialog."""
        mpv = self.get_main_photo_viewer()
        return mpv.get_children_by_type("ViewerChrome")[0].get_children_by_type("PopupMenu")[2]

    def get_photo_component(self):
        mpv = self.get_main_photo_viewer()
        return mpv.get_children_by_type("QQuickItem")[0].get_children_by_type("ZoomablePhotoComponent")[0]

    def get_crop_menu_item(self):
        """Returns the crop item of the edit dialog."""
        edit_dialog = self.get_photo_edit_dialog()
        return edit_dialog.get_children_by_type("QQuickColumn")[0].get_children_by_type("MenuItem")[1]



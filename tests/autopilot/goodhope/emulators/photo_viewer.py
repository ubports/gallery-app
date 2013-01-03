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
        return self.app.select_single("OrganicMediaList", objectName="eventViewPhoto0", visible=True)

    def get_main_photo_viewer(self):
        """Returns the PhotoViewer."""
        return self.app.select_single("PhotoViewer", objectName="photoViewer")

    def get_crop_interactor(self):
        """Returns the crop interactor."""
        return self.app.select_single("CropInteractor", objectName="cropInteractor")

    def get_viewer_chrome_toolbar_buttons(self):
        """Returns the toolbar buttons."""
        mpv = self.get_photo_viewer_chrome()
        gst = mpv.get_children_by_type("QQuickMouseArea")[0].get_children_by_type("QQuickItem")[0]
        return gst.get_children_by_type("QQuickColumn")[0].get_children_by_type("QQuickItem")[0]

    def get_viewer_chrome_toolbar_edit_button(self):
        """Returns the edit button of the photo viewer toolbar."""
        return self.app.select_single("ChromeButton", objectName="edit", visible=True)

    def get_photo_viewer_chrome(self):
        return self.app.select_single("ChromeBar", objectName="photoViewerChrome")

    def get_photo_viewer(self):
        return self.app.select_single("PhotoViewer", objectName='photoViewer', currentIndex=0)

    def get_viewer_chrome_back_button(self):
        """Returns the photo viewer back button."""
        return self.app.select_single("ChromeButton", objectName="backButton0", visible=True)

    def get_viewer_chrome_trash_button(self):
        """Returns the photo viewer trash button."""
        return self.app.select_single("ChromeButton", objectName='delete', visible=True)

    def get_viewer_chrome_album_button(self):
        """Returns the photo viewer album button."""
        return self.app.select_single("ChromeButton", objectName='disabled', visible=True)

    def get_viewer_chrome_share_button(self):
        """Returns the photo viewer share button."""
        return self.app.select_single("ChromeButton", objectName='share', visible=True)

    def get_viewer_chrome_more_button(self):
        """Returns the photo viewer 'more items' button."""
        qqr = self.app.select_single("QQuickRow", objectName="viewerChromeButtons")
        return qqr.get_children_by_type("ChromeButton")[2]

    def get_delete_dialog(self):
        """Returns the photo viewer delete dialog."""
        return self.app.select_single("DeletePopover", objectName="deletePopover")

    def get_popup_album_picker(self):
        """Returns the photo viewer album pickers."""
        return self.app.select_single("PopupAlbumPicker", objectName="popupAlbumPicker")

    def get_share_dialog(self):
        """Returns the photo viewer share dialog."""
        return self.app.select_single("SharePopover", objectName="sharePopover")

    def get_photo_edit_dialog(self):
        """Returns the photo edit dialog."""
        return self.app.select_single("EditPopover", objectName="editPopover")

    def get_photo_component(self):
        return self.app.select_many("ZoomablePhotoComponent", ownerName="photoViewerDelegate")[0]

    def get_crop_menu_item(self):
        """Returns the crop item of the edit dialog."""
        return self.app.select_single("Standard", objectName="cropListItem")



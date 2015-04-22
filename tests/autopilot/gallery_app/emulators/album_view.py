# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import logging
import re

from testtools.matchers import GreaterThan, LessThan, Equals, Is
from autopilot.matchers import Eventually

from autopilot import logging as autopilot_logging

from gallery_app.emulators.gallery_utils import(
    GalleryUtils,
    GalleryAppException,
)

logger = logging.getLogger(__name__)


class AlbumView(GalleryUtils):
    """An emulator class that makes it easy to interact with the gallery app"""

    def __init__(self, app):
        super(AlbumView, self).__init__(self)
        self.app = app

    def get_animated_album_view(self):
        """Returns the album view"""
        return self.app.wait_select_single("AlbumViewerAnimated",
                                           objectName="albumViewerAnimated")

    def get_album_view(self):
        """Returns the album view"""
        return self.app.wait_select_single("AlbumViewer",
                                           objectName="albumViewer")

    def get_first_photo(self):
        """Returns the first photo in a newly opened album"""
        return self.app.select_many(
            "FramePortrait", visible=True, isLoaded=True)[0]

    def get_album_photo_view(self):
        """Returns the photo view of the album viewer"""
        return self.app.select_single("PopupPhotoViewer")

    def get_spread_view(self):
        """Returns the inner spread view to access the pages"""
        view = self.get_album_view()
        return view.select_single(
            "AlbumSpreadViewer",
            objectName="spreadViewer"
        )

    def number_of_photos(self):
        """Returns the numer of visible photos"""
        return len(self.app.select_many("FramePortrait",
                                        visible=True, isLoaded=True))

    def media_selector_loader(self):
        """Returns the loader for the media selector"""
        return self.app.select_single("QQuickLoader",
                                      objectName="albumMediaSelectorLoader")

    def get_plus_icon_empty_album(self):
        """Returns the plus icon visible in empty albums"""
        return self.app.select_single("QQuickImage",
                                      objectName="albumCoverAddPhotosImage",
                                      visible=True)

    def ensure_animated_fully_closed(self):
        """Ensure the animated album view is fully closed"""
        animated_viewer = self.get_animated_album_view()
        animated_viewer.isOpen.wait_for(False)
        animated_viewer.animationRunning.wait_for(False)

    def ensure_album_view_fully_closed(self):
        """Ensure the album view is fully closed"""
        view = self.get_album_view()
        view.visible.wait_for(False)

    def ensure_media_selector_is_fully_closed(self):
        """Ensure media selector is fully closed"""
        loader = self.media_selector_loader()
        loader.status.wait_for(0)

    def _swipe_setup(self, page_number, direction):
        self.album = self.get_album_view()
        self.spread = self.get_spread_view()
        self.album.animationRunning.wait_for(False)

        x, y, w, h = self.spread.globalRect
        mid_y = y + h // 2
        mid_x = x + w // 2

        if 'left' == direction:
            matcher = LessThan
            self.pointing_device.drag(
                mid_x, mid_y,  # Start
                x + w, mid_y,  # Finish
                rate=1
            )

        elif 'right' == direction:
            matcher = GreaterThan
            self.pointing_device.drag(
                mid_x, mid_y,  # Start
                x, mid_y,      # Finish
                rate=1
            )
        else:
            raise GalleryAppException(
                'Direction can be "right" or "left" '
                'you choose "{}"'.format(direction))

        self.album.animationRunning.wait_for(False)
        self.spread.viewingPage.wait_for(matcher(page_number))

    def swipe_page_left(self, page_number):
        '''Swipe page to the left

        :param page_number: The starting page number you are swiping from
        '''
        self._swipe_setup(page_number, 'left')

    def swipe_page_right(self, page_number):
        '''Swipe page to the right

        :param page_number: The starting page number you are swiping from
        '''
        self._swipe_setup(page_number, 'right')

    def _get_remove_from_album_dialog(self):
        """Returns the photo viewer remove from album dialog."""
        return self.app.wait_select_single(
            "Dialog",
            objectName="removePhotoFromAlbumDialog")

    def _remove_from_album_dialog_shown(self):
        dialog = self.app.select_many("Dialog",
                                      objectName="removePhotoFromAlbumDialog")
        return len(dialog) >= 1

    def _get_remove_from_album_popover_remove_item(self):
        """Returns remove button of the remove from album popover."""
        return self.app.wait_select_single("Button",
                                           objectName="removeFromAlbumButton",
                                           visible=True)

    def _get_remove_from_album_popover_delete_item(self):
        """Returns delete button of the remove from album popover."""
        return self.app.wait_select_single(
            "Button",
            objectName="removeFromAlbumAndDeleteButton",
            visible=True)

    def _get_remove_from_album_popover_cancel_item(self):
        """Returns cancel button of the remove from album popover."""
        return self.app.wait_select_single(
            "Button",
            objectName="removeFromAlbumCancelButton",
            visible=True)

    def _ensure_remove_from_album_dialog_is_open(self):
        """Ensure that the remove from album dialog is fully opened."""
        self.assertThat(self._remove_from_album_dialog_shown,
                        Eventually(Is(True)))
        remove_dialog = self._get_remove_from_album_dialog()
        self.assertThat(remove_dialog.visible, Eventually(Equals(True)))
        self.assertThat(remove_dialog.opacity, Eventually(Equals(1)))

    def _ensure_remove_from_album_dialog_is_close(self):
        """Ensure that the remove from album dialog is fully closed."""
        self.assertThat(self._remove_from_album_dialog_shown,
                        Eventually(Is(False)))

    @autopilot_logging.log_action(logger.info)
    def click_remove_from_album_remove_button(self):
        """Click on the remove from album button of the remove dialog."""
        self._ensure_remove_from_album_dialog_is_open()

        remove_item = self._get_remove_from_album_popover_remove_item()
        self.pointing_device.click_object(remove_item)

        self._ensure_remove_from_album_dialog_is_close()

    @autopilot_logging.log_action(logger.info)
    def click_remove_from_album_delete_button(self):
        """Click on the remove and delete of the remove dialog."""
        self._ensure_remove_from_album_dialog_is_open()

        delete_item = self._get_remove_from_album_popover_delete_item()
        self.pointing_device.click_object(delete_item)

        self._ensure_remove_from_album_dialog_is_close()

    @autopilot_logging.log_action(logger.info)
    def click_remove_from_album_cancel_button(self):
        """Click on the cancel of the remove dialog."""
        self._ensure_remove_from_album_dialog_is_open()

        cancel_item = self._get_remove_from_album_popover_cancel_item()
        self.pointing_device.click_object(cancel_item)

        self._ensure_remove_from_album_dialog_is_close()

    @autopilot_logging.log_action(logger.info)
    def click_first_photo(self):
        """Click on the first photo of the album and return the path of it."""
        photo = self.get_first_photo()
        images = photo.select_many('QQuickImage')
        path = ''
        for i in images:
            if str(i.source).startswith('image://thumbnailer/file://'):
                path = re.sub('^image://thumbnailer/file://', '',
                              i.source).split('?')[0]
        self.pointing_device.click_object(photo)
        return path

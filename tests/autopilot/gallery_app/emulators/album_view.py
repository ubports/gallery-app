# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from testtools.matchers import GreaterThan, LessThan

from gallery_app.emulators.gallery_utils import(
    GalleryUtils,
    GalleryAppException,
)


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

    def get_toolbar_add_button(self):
        """Returns the add button of the tollbar in the events view."""
        return self.get_toolbar_named_toolbarbutton("addButton")

    def get_first_photo(self):
        """Returns the first photo in a newly opened album"""
        return self.app.select_many(
            "FramePortrait", visible=True, isLoaded=True)[0]

    def get_album_photo_view(self):
        """Returns the photo view of the album viewer"""
        view = self.get_album_view()
        return view.select_single("PopupPhotoViewer")

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
                                      objectName="addButton", visible=True)

    def ensure_animated_fully_closed(self):
        """Ensure the animated album view is fully closed"""
        animated_viewer = self.get_animated_album_view()
        animated_viewer.isOpen.wait_for(False)
        animated_viewer.animationRunning.wait_for(False)

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
                x + w, mid_y  # Finish
            )

        elif 'right' == direction:
            matcher = GreaterThan
            self.pointing_device.drag(
                mid_x, mid_y,  # Start
                x, mid_y  # Finish
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

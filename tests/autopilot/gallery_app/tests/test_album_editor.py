# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the album editor of the gallery app"""

from __future__ import absolute_import

from testtools.matchers import Equals
from autopilot.matchers import Eventually

from gallery_app.emulators.album_view import AlbumView
from gallery_app.emulators.media_selector import MediaSelector
from gallery_app.emulators import album_editor
from gallery_app.tests import GalleryTestCase


class TestAlbumEditor(GalleryTestCase):
    """Tests the album editor of the gallery app"""

    @property
    def album_view(self):
        return AlbumView(self.app)

    @property
    def media_selector(self):
        return MediaSelector(self.app)

    def setUp(self):
        self.ARGS = []
        super(TestAlbumEditor, self).setUp()
        self.switch_to_albums_tab()
        self.main_view.close_toolbar()
        self.edit_first_album()

    def edit_first_album(self):
        first_album = self.gallery_utils.get_first_album()
        self.tap_item(first_album)
        edit_button = self.gallery_utils.get_edit_album_button()
        self.click_item(edit_button)
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        editor.ensure_fully_open()

    def ensure_media_selector_is_fully_open(self):
        media_selector = self.media_selector.get_media_selector()
        self.assertThat(media_selector.opacity, Eventually(Equals(1.0)))

    def ensure_album_viewer_is_fully_closed(self):
        animated_viewer = self.album_view.get_animated_album_view()
        self.assertThat(animated_viewer.isOpen, Eventually(Equals(False)))
        self.assertThat(animated_viewer.animationRunning,
                        Eventually(Equals(False)))

    def test_album_title_fields(self):
        """tests the title and sub title"""
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        title_field = editor.album_title_entry_field()
        subtitle_field = editor.album_subtitle_entry_field()

        text = "My Photo Album"
        self.assertThat(title_field.text, Eventually(Equals(text)))

        text = "Ubuntu"
        self.assertThat(subtitle_field.text, Eventually(Equals(text)))

        self.main_view.close_toolbar()
        editor.click_title_field()
        self.keyboard.press_and_release("Ctrl+a")
        self.keyboard.press_and_release("P")
        self.keyboard.press_and_release("h")
        self.keyboard.press_and_release("o")
        self.keyboard.press_and_release("t")
        self.keyboard.press_and_release("o")
        self.keyboard.press_and_release("s")
        text = "Photos"
        #due to some reason the album title is not updated unless it loses the
        #focus. So we click on the subtitle field.
        editor.click_subtitle_field()
        self.assertThat(title_field.text, Eventually(Equals(text)))

        self.keyboard.press_and_release("Ctrl+a")
        self.keyboard.press_and_release("U")
        self.keyboard.press_and_release("1")
        text = "U1"
        editor.click_title_field()
        self.assertThat(subtitle_field.text, Eventually(Equals(text)))

    def test_add_photo(self):
        """Tests adding a photo using the media selector"""
        # first open, but cancel before adding a photo
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        editor.add_photos()
        self.ensure_media_selector_is_fully_open()

        self.main_view.open_toolbar().click_custom_button("cancelButton")
        editor.ensure_fully_closed()

        self.main_view.close_toolbar()
        self.open_first_album()
        num_photos_start = self.album_view.number_of_photos()
        self.assertThat(num_photos_start, Equals(1))
        self.main_view.open_toolbar().click_button("backButton")
        self.ensure_album_viewer_is_fully_closed()

        # now open to add a photo
        self.edit_first_album()
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        self.main_view.close_toolbar()
        editor.add_photos()
        self.ensure_media_selector_is_fully_open()

        photo = self.media_selector.get_second_photo()
        self.click_item(photo)
        self.main_view.get_toolbar().click_custom_button("addButton")
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        editor.ensure_fully_closed()

        self.main_view.close_toolbar()
        self.open_first_album()
        num_photos = self.album_view.number_of_photos()
        self.assertThat(num_photos, Equals(num_photos_start + 1))

    def test_cover_image(self):
        """Test to change the album cover image"""
        editor = self.app.select_single(album_editor.AlbumEditorAnimated)
        cover_image = editor.album_cover_image()
        self.assertThat(
            cover_image.source.endswith("album-cover-default-large.png"),
            Equals(True))
        self.main_view.close_toolbar()

        # click somewhere rather at the bottom of the cover
        x, y, w, h = cover_image.globalRect
        self.pointing_device.move(x + int(w / 2), y + h - int(h / 10))
        self.pointing_device.click()

        green_item = self.gallery_utils.get_cover_menu_item(2)
        self.click_item(green_item)

        self.assertThat(lambda:
            cover_image.source.endswith("album-cover-green-large.png"),
            Eventually(Equals(True)))

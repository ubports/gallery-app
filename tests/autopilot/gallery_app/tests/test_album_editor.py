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

from gallery_app.emulators.album_editor import AlbumEditor
from gallery_app.emulators.album_view import AlbumView
from gallery_app.emulators.media_selector import MediaSelector
from gallery_app.tests import GalleryTestCase


class TestAlbumEditor(GalleryTestCase):
    """Tests the album editor of the gallery app"""

    @property
    def album_editor(self):
        return AlbumEditor(self.app)

    @property
    def album_view(self):
        return AlbumView(self.app)

    @property
    def media_selector(self):
        return MediaSelector(self.app)

    def setUp(self):
        super(TestAlbumEditor, self).setUp()
        self.switch_to_albums_tab()
        self.edit_first_album()

    def edit_first_album(self):
       first_album = self.album_editor.get_first_album()
       self.tap_item(first_album)
       edit_button = self.album_editor.get_edit_album_button()
       self.click_item(edit_button)
       self.ensure_edit_is_fully_open()

    def ensure_edit_is_fully_open(self):
        animated_editor = self.album_editor.get_animated_album_editor()
        self.assertThat(animated_editor.isOpen, Eventually(Equals(True)))
        editor = self.album_editor.get_album_editor()
        self.assertThat(editor.visible, Eventually(Equals(True)))
        self.assertThat(animated_editor.animationRunning, Eventually(Equals(False)))

    def click_title_field(self):
        title_field = self.album_editor.get_album_title_entry_field()
        self.click_item(title_field)

    def click_subtitle_field(self):
        subtitle_field = self.album_editor.get_album_subtitle_entry_field()
        self.click_item(subtitle_field)

    def ensure_media_selector_is_fully_open(self):
        media_selector = self.media_selector.get_media_selector()
        self.assertThat(media_selector.opacity, Eventually(Equals(1.0)))

    def ensure_album_editor_is_fully_closed(self):
        animated_editor = self.album_editor.get_animated_album_editor()
        self.assertThat(animated_editor.isOpen, Eventually(Equals(False)))
        self.assertThat(animated_editor.animationRunning, Eventually(Equals(False)))

    def ensure_album_viewer_is_fully_closed(self):
        animated_viewer = self.album_editor.get_animated_album_view()
        self.assertThat(animated_viewer.isOpen, Eventually(Equals(False)))
        self.assertThat(animated_viewer.animationRunning, Eventually(Equals(False)))

    def test_album_title_fields(self):
        """tests the title and sub title"""
        title_field = self.album_editor.get_album_title_entry_field()
        subtitle_field = self.album_editor.get_album_subtitle_entry_field()

        text = "My Photo Album"
        self.assertThat(title_field.text, Equals(text))

        text = "Ubuntu"
        self.assertThat(subtitle_field.text, Equals(text))

        self.click_title_field()
        self.keyboard.press_and_release("Ctrl+a")
        self.keyboard.press_and_release("P")
        self.keyboard.press_and_release("h")
        self.keyboard.press_and_release("o")
        self.keyboard.press_and_release("t")
        self.keyboard.press_and_release("o")
        self.keyboard.press_and_release("s")
        text = "Photos"
        #due to some reason the album title is not updated unless it loses the focus
        #so we click on the subtitle field.
        self.click_subtitle_field()
        self.assertThat(title_field.text, Equals(text))

        self.keyboard.press_and_release("Ctrl+a")
        self.keyboard.press_and_release("U")
        self.keyboard.press_and_release("1")
        text = "U1"
        self.click_title_field()
        self.assertThat(subtitle_field.text, Equals(text))

    def test_add_photo(self):
        """Tests adding a photo using the media selector"""
        # first open, but cancel before adding a photo
        plus_icon = self.album_editor.get_plus_icon()
        self.click_item(plus_icon)
        self.ensure_media_selector_is_fully_open()

        cancel = self.media_selector.get_toolbar_cancel_icon()
        self.click_item(cancel)
        self.ensure_album_editor_is_fully_closed()

        self.open_first_album()
        num_photos_start = self.album_view.number_of_photos()
        self.assertThat(num_photos_start, Equals(1))
        self.reveal_toolbar()
        cancel = self.album_view.get_toolbar_cancel_icon()
        self.click_item(cancel)
        self.ensure_album_editor_is_fully_closed()

        # now open to add a photo
        self.edit_first_album()
        plus_icon = self.album_editor.get_plus_icon()
        self.click_item(plus_icon)
        self.ensure_media_selector_is_fully_open()

        photo = self.media_selector.get_second_photo()
        self.click_item(photo)
        add_button = self.media_selector.get_toolbar_add_button()
        self.click_item(add_button)
        self.ensure_album_editor_is_fully_closed()

        self.open_first_album()
        num_photos = self.album_view.number_of_photos()
        self.assertThat(num_photos, Equals(num_photos_start + 1))

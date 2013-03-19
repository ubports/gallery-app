# # -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# # Copyright 2012 Canonical
# #
# # This program is free software: you can redistribute it and/or modify it
# # under the terms of the GNU General Public License version 3, as published
# # by the Free Software Foundation.


# """Tests for the Gallery App"""

# from __future__ import absolute_import

# from testtools.matchers import Equals, NotEquals
# from autopilot.matchers import Eventually

# from gallery_app.tests import GalleryTestCase


# class TestAlbumEditor(GalleryTestCase):
#     """Tests the album editor of the gallery app"""

#     """ This is needed to wait for the application to start.
#         In the testfarm, the application may take some time to show up."""
#     def setUp(self):
#         super(TestAlbumEditor, self).setUp()
#         self.assertThat(self.album_editor.get_qml_view().visible, Eventually(Equals(True)))

#         self.click_albums_tab()
#         self.click_plus_icon()

#     def tearDown(self):
#         super(TestAlbumEditor, self).tearDown()

#     def click_albums_tab(self):
#         albums_tab = self.album_editor.get_albums_tab()

#         self.pointing_device.move_to_object(albums_tab)
#         self.pointing_device.click()

#         self.assertThat(albums_tab.state, Eventually(Equals("selected")))

#     def click_plus_icon(self):
#         add_icon = self.album_editor.get_plus_icon()

#         self.pointing_device.move_to_object(add_icon)
#         self.pointing_device.click()

#     def click_title_field(self):
#         title_field = self.album_editor.get_album_title_entry_field()

#         self.pointing_device.move_to_object(title_field)
#         self.pointing_device.click()

#     def click_subtitle_field(self):
#         subtitle_field = self.album_editor.get_album_subtitle_entry_field()

#         self.pointing_device.move_to_object(subtitle_field)
#         self.pointing_device.click()

#     def test_album_title_field_default_text(self):
#         """Ensures the default text of the title field is intact."""
#         title_field = self.album_editor.get_album_title_entry_field()

#         self.assertThat(title_field.text, Eventually(Equals("New Photo Album")))

#     def test_album_subtitle_field_default_text(self):
#         """Ensures the default text of the subtitle field is intact."""
#         subtitle_field = self.album_editor.get_album_subtitle_entry_field()

#         self.assertThat(subtitle_field.text, Eventually(Equals("Subtitle")))

#     def test_album_title_entry_field(self):
#         """Ensures text can be removed from the title input field."""
#         title_field = self.album_editor.get_album_title_entry_field()
#         subtitle_field = self.album_editor.get_album_subtitle_entry_field()

#         self.click_title_field()

#         self.keyboard.press_and_release("Ctrl+a")
#         self.keyboard.press_and_release("Delete")

#         #due to some reason the album title is not updated unless it loses the focus
#         #so we click on the subtitle field.
#         self.click_subtitle_field()

#         self.assertThat(title_field.text, Eventually(Equals("")))

#     def test_album_title_entry_input(self):
#         """Ensures text can be inserted into the title input field."""
#         title_field = self.album_editor.get_album_title_entry_field()
#         subtitle_field = self.album_editor.get_album_subtitle_entry_field()

#         self.click_title_field()

#         self.keyboard.press_and_release("Ctrl+a")
#         self.keyboard.type("test")

#         self.click_subtitle_field()

#         self.assertThat(title_field.text, Eventually(Equals("test")))

#     def test_album_subtitle_entry_field(self):
#         """Ensures text can be removed from the subtitle input field."""
#         title_field = self.album_editor.get_album_title_entry_field()
#         subtitle_field = self.album_editor.get_album_subtitle_entry_field()

#         self.click_subtitle_field()

#         self.keyboard.press_and_release("Ctrl+a")
#         self.keyboard.press_and_release("Delete")

#         self.click_title_field()

#         self.assertThat(subtitle_field.text, Eventually(Equals("")))

#     def test_album_subtitle_entry_input(self):
#         """Ensures text can be inserted into the subtitle input field."""
#         title_field = self.album_editor.get_album_title_entry_field()
#         subtitle_field = self.album_editor.get_album_subtitle_entry_field()

#         self.click_subtitle_field()

#         self.keyboard.press_and_release("Ctrl+a")
#         self.keyboard.type("test")

#         self.click_title_field()

#         self.assertThat(subtitle_field.text, Eventually(Equals("test")))


# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012-2015 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


"""Tests the Photo editor of the gallery app."""

from testtools.matchers import Equals, NotEquals, GreaterThan
from autopilot.matchers import Eventually
from testtools import skipIf
from autopilot.platform import model

from gallery_app.emulators import photo_viewer
from gallery_app.emulators.media_viewer import MediaViewer
from gallery_app.emulators.events_view import EventsView
from gallery_app.tests import GalleryTestCase

import os
import shutil
from time import sleep
import unittest

"""
Class for common functionality of the photo viewing and photo editing
"""


class TestMediaViewerBase(GalleryTestCase):
    @property
    def photo_viewer(self):
        return photo_viewer.PhotoViewer(self.app)

    @property
    def events_view(self):
        return EventsView(self.app)

    def setUp(self):
        self.ARGS = []
        super(TestMediaViewerBase, self).setUp()
        self.main_view.switch_to_tab("eventsTab")

    def open_first_photo(self):
        self.assertThat(
            lambda: self.events_view.number_of_photos_in_events(),
            Eventually(GreaterThan(0))
        )

        self.events_view.click_photo(self.sample_file)

        photo_viewer_loader = self.photo_viewer.get_main_photo_viewer_loader()
        self.assertThat(photo_viewer_loader.loaded, Eventually(Equals(True)))
        sleep(1)
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))


class TestPhotoViewer(TestMediaViewerBase):
    def setUp(self):
        super(TestPhotoViewer, self).setUp()
        self.open_first_photo()

    @unittest.skip("Temporarily disable as it fails in some cases, "
                   "supposedly due to problems with the infrastructure")
    def test_save_state(self):
        """Quitting the app once a photo has been opened will return
        to that same photo on restart"""
        path = self.photo_viewer.get_photo_component().select_single(
            "QQuickImage").source

        self.ensure_app_has_quit()
        self.start_app()

        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))
        new_path = self.photo_viewer.get_photo_component().select_single(
            "QQuickImage").source

        self.assertThat(path, Equals(new_path))

    @unittest.skip("Temporarily disable as it fails in some cases, "
                   "supposedly due to problems with the infrastructure")
    def test_no_save_state_on_back(self):
        """Quitting the app once a photo has been opened and then closed
        will not reopen a photo on restart"""
        self.main_view.open_toolbar().click_button("backButton")

        self.ensure_app_has_quit()
        self.start_app()

        photo_viewer_loader = self.photo_viewer.get_main_photo_viewer_loader()
        self.assertThat(photo_viewer_loader.source, Equals(""))

    def test_nav_bar_back_button(self):
        """Clicking the back button must close the photo."""
        self.main_view.get_header().click_custom_back_button()
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(False)))

    @skipIf(model() == 'Desktop', 'Share not supported on desktop')
    def test_share_button(self):
        """Clicking the share button must show the ContentPeerPicker."""
        self.main_view.get_header().click_action_button("shareButton")
        share_picker = self.photo_viewer.get_share_peer_picker()
        self.assertThat(share_picker.visible, Eventually(Equals(True)))
        cancel_button = \
            self.photo_viewer.get_content_peer_picker_cancel_button()
        self.click_item(cancel_button)
        self.assertThat(share_picker.visible, Eventually(Equals(False)))

    def test_delete_photo_must_remove_it_from_filesystem(self):
        photo_component = self.main_view.select_single('GalleryPhotoComponent')
        source = photo_component.source
        file_path = source[len('image://thumbnailer/file://'):]
        self.assertTrue(os.path.exists(file_path))

        photo_viewer_popup = self.main_view.select_single(
            photo_viewer.PopupPhotoViewer)
        photo_viewer_popup.delete_current_photo()

        self.assertFalse(os.path.exists(file_path))

    def test_photo_delete_works(self):
        """Clicking the trash button must show the delete dialog."""
        # XXX This test must be split into multiple QML tests.
        # --elopio - 2015-14-16
        photo_viewer_popup = self.main_view.select_single(
            photo_viewer.PopupPhotoViewer)
        photo_viewer_popup.delete_current_photo(confirm=False)

        self.assertThat(lambda: os.path.exists(self.sample_file),
                        Eventually(Equals(True)))

        photo_viewer_popup.delete_current_photo()
        self.assertThat(lambda: os.path.exists(self.sample_file),
                        Eventually(Equals(False)))

        # Delete all other pictures and make sure the photo viewer closes
        photo_viewer_popup.delete_current_photo()
        photo_viewer_popup.delete_current_photo()
        photo_viewer_popup.delete_current_photo()
        photo_viewer_popup.delete_current_photo()

        self.assertThat(photo_viewer_popup.visible, Eventually(Equals(False)))

    def test_nav_bar_album_picker_button(self):
        """Clicking the album picker must show the picker dialog."""
        self.main_view.get_header().click_action_button("addButton")
        album_picker = self.photo_viewer.get_popup_album_picker()
        self.assertThat(album_picker.visible, Eventually(Equals(True)))

    def test_double_click_zoom(self):
        """Double clicking an opened photo must zoom it."""
        opened_photo = self.photo_viewer.get_photo_component()

        self.pointing_device.move_to_object(opened_photo)
        self.pointing_device.click()
        self.pointing_device.click()

        self.assertThat(opened_photo.fullyZoomed, Eventually(Equals(True)))

        self.pointing_device.click()
        self.pointing_device.click()

        self.assertThat(opened_photo.fullyUnzoomed, Eventually(Equals(True)))

    def test_swipe_change_image(self):
        """Swiping left and right on a photo should move to another photo"""
        list = self.photo_viewer.get_photos_list()
        self.assertThat(list.currentIndex, Eventually(Equals(0)))

        # Slide left should move to the next image
        x, y, w, h = list.globalRect
        mid_y = y + h // 2
        mid_x = x + w // 2
        self.pointing_device.drag(mid_x, mid_y, x + 10, mid_y, rate=5)

        self.assertThat(list.moving, Eventually(Equals(False)))
        self.assertThat(list.currentIndex, Eventually(Equals(1)))

        # Slide right should get us back to the start
        self.pointing_device.drag(mid_x, mid_y, x + w - 10, mid_y, rate=5)

        self.assertThat(list.moving, Eventually(Equals(False)))
        self.assertThat(list.currentIndex, Eventually(Equals(0)))

        # Slide right again shouldn't go anywhere
        self.pointing_device.drag(mid_x, mid_y, x + w - 10, mid_y, rate=5)

        self.assertThat(list.moving, Eventually(Equals(False)))
        self.assertThat(list.currentIndex, Eventually(Equals(0)))


class TestVideoViewer(TestMediaViewerBase):
    def test_video_loads_thumbnail(self):
        num_events = self.events_view.number_of_events()

        video_file = "video.mp4"
        shutil.copyfile(self.sample_dir+"/option01/"+video_file,
                        self.sample_destination_dir+"/"+video_file)

        self.assertThat(
            lambda: self.events_view.number_of_events(),
            Eventually(Equals(num_events + 1))
        )

        self.events_view.click_photo(video_file)

        photo_viewer_loader = self.photo_viewer.get_main_photo_viewer_loader()
        self.assertThat(photo_viewer_loader.loaded, Eventually(Equals(True)))
        sleep(1)
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))
        photo_component = self.photo_viewer.get_photo_component()
        self.assertThat(photo_component.imageReady, Eventually(Equals(True)))

        os.remove(self.sample_destination_dir+"/"+video_file)


class TestPhotoEditor(TestMediaViewerBase):

    def setUp(self):
        super(TestPhotoEditor, self).setUp()
        self.open_first_photo()
        self.click_edit_button()
        self.media_view = self.app.select_single(MediaViewer)

    def click_edit_button(self):
        self.main_view.get_header().click_action_button("editButton")
        photo_editor = self.photo_viewer.get_photo_editor()
        self.assertThat(photo_editor.visible, (Eventually(Equals(True))))
        self.assertThat(photo_editor.opacity, (Eventually(Equals(1))))

    def test_photo_editor_crop(self):
        """Cropping a photo must crop it."""
        old_file_size = os.path.getsize(self.sample_file)

        self.photo_viewer.click_crop_button()

        crop_box = self.photo_viewer.get_crop_interactor()
        item_width = crop_box.width
        item_height = crop_box.height

        self.assertThat(crop_box.opacity, Eventually(Equals(1)))

        crop_corner = self.photo_viewer.get_top_left_crop_corner()
        x, y, h, w = crop_corner.globalRect
        x = x + w // 2
        y = y + h // 2
        self.pointing_device.drag(x, y,
                                  x + item_width // 2, y + item_height // 2)

        # wait for animation being finished
        crop_overlay = self.photo_viewer.get_crop_overlay()
        self.assertThat(crop_overlay.interpolationFactor,
                        Eventually(Equals(1.0)))

        crop_button = self.photo_viewer.get_crop_overlays_crop_icon()
        self.click_item(crop_button)
        self.media_view.ensure_spinner_not_running()

        self.main_view.get_header().click_custom_back_button()
        photo_viewer = self.photo_viewer.get_main_photo_viewer()
        self.assertThat(photo_viewer.visible, Eventually(Equals(True)))

        new_file_size = os.path.getsize(self.sample_file)
        self.assertThat(old_file_size > new_file_size, Equals(True))

    def test_photo_editor_rotate(self):
        """Makes sure that the photo editor inside the photo viewer works using
           the rotate function"""
        opened_photo = self.photo_viewer.get_opened_photo()
        item_height = opened_photo.height

        def is_landscape():
            return opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape(), Equals(True))

        self.photo_viewer.click_rotate_button()
        self.media_view.ensure_spinner_not_running()
        self.main_view.get_header().click_custom_back_button()

        self.assertThat(opened_photo.paintedHeight,
                        Eventually(Equals(item_height)))
        self.assertThat(lambda: is_landscape(),
                        Eventually(Equals(False)))

        self.click_edit_button()
        self.photo_viewer.click_rotate_button()
        self.media_view.ensure_spinner_not_running()
        self.main_view.get_header().click_custom_back_button()

        self.click_edit_button()
        self.photo_viewer.click_revert_button()
        self.photo_viewer.click_confirm_revert_button()
        self.main_view.get_header().click_custom_back_button()

        self.assertThat(opened_photo.paintedHeight,
                        Eventually(NotEquals(item_height)))
        is_landscape = opened_photo.paintedWidth > opened_photo.paintedHeight
        self.assertThat(is_landscape, Equals(True))

    @unittest.skip("Temporarily disable as the enhance feature is not "
                   "implemented on the new editor")
    def test_photo_editor_redo_undo_revert_enhance_states(self):
        undo_item = self.photo_viewer.get_undo_menu_item()
        redo_item = self.photo_viewer.get_redo_menu_item()
        revert_item = self.photo_viewer.get_revert_menu_item()

        self.assertThat(undo_item.enabled, Eventually(Equals(False)))
        self.assertThat(redo_item.enabled, Eventually(Equals(False)))
        self.assertThat(revert_item.enabled, Eventually(Equals(False)))

        self.photo_viewer.click_rotate_item()
        self.media_view.ensure_spinner_not_running()

        self.click_edit_button()
        undo_item = self.photo_viewer.get_undo_menu_item()
        redo_item = self.photo_viewer.get_redo_menu_item()
        revert_item = self.photo_viewer.get_revert_menu_item()

        self.assertThat(undo_item.enabled, Eventually(Equals(True)))
        self.assertThat(redo_item.enabled, Eventually(Equals(False)))
        self.assertThat(revert_item.enabled, Eventually(Equals(True)))

        self.photo_viewer.click_undo_item()
        self.media_view.ensure_spinner_not_running()

        self.click_edit_button()
        undo_item = self.photo_viewer.get_undo_menu_item()
        redo_item = self.photo_viewer.get_redo_menu_item()
        revert_item = self.photo_viewer.get_revert_menu_item()

        self.assertThat(undo_item.enabled, Eventually(Equals(False)))
        self.assertThat(redo_item.enabled, Eventually(Equals(True)))
        self.assertThat(revert_item.enabled, Eventually(Equals(False)))

        self.photo_viewer.click_redo_item()
        self.media_view.ensure_spinner_not_running()

        self.click_edit_button()
        undo_item = self.photo_viewer.get_undo_menu_item()
        redo_item = self.photo_viewer.get_redo_menu_item()
        revert_item = self.photo_viewer.get_revert_menu_item()

        self.assertThat(undo_item.enabled, Eventually(Equals(True)))
        self.assertThat(redo_item.enabled, Eventually(Equals(False)))
        self.assertThat(revert_item.enabled, Eventually(Equals(True)))

        self.photo_viewer.click_revert_item()
        self.media_view.ensure_spinner_not_running()

        self.click_edit_button()
        undo_item = self.photo_viewer.get_undo_menu_item()
        redo_item = self.photo_viewer.get_redo_menu_item()
        revert_item = self.photo_viewer.get_revert_menu_item()

        self.assertThat(undo_item.enabled, Eventually(Equals(True)))
        self.assertThat(redo_item.enabled, Eventually(Equals(False)))
        self.assertThat(revert_item.enabled, Eventually(Equals(False)))

        self.photo_viewer.click_enhance_item()
        self.media_view.ensure_spinner_not_running()

        self.click_edit_button()

        revert_item = self.photo_viewer.get_revert_menu_item()
        self.assertThat(lambda: revert_item.enabled, Eventually(Equals(True)))

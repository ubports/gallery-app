# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""gallery autopilot tests."""

import logging
import os.path
import shutil

from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from testtools.matchers import Equals, GreaterThan, Not, Is

from ubuntuuitoolkit import emulators as toolkit_emulators
from gallery_app.emulators import main_screen
from gallery_app.emulators.gallery_utils import GalleryUtils

from time import sleep

logger = logging.getLogger(__name__)


class EnvironmentTypes:
    installed = "installed"
    local = "local"
    click = "click"


class GalleryTestCase(AutopilotTestCase):

    """A common test case class that provides several useful methods for
       gallery tests."""

    sample_file_source = "/sample01.jpg"
    tap_press_time = 1
    local_location = "../../src/gallery-app"

    _default_sample_destination_dir = "/tmp/gallery-ap_sd"

    _sample_dirs = {
        EnvironmentTypes.installed:
        "/usr/lib/python2.7/dist-packages/gallery_app/data",
        EnvironmentTypes.local: "gallery_app/data",
        EnvironmentTypes.click: "gallery_app/data",
    }

    ARGS = []

    @property
    def gallery_utils(self):
        return GalleryUtils(self.app)

    @property
    def main_view(self):
        return self.app.select_single(main_screen.MainScreen)

    def _get_environment_launch_type(self):
        """Returns the type of the environment in which to setup and launch
        gallery-app.

        """
        # Lets assume we are installed system wide if this file is somewhere
        # in /usr
        if os.path.realpath(__file__).startswith("/usr/"):
            return EnvironmentTypes.installed
        else:
            if os.path.exists(self.local_location):
                return EnvironmentTypes.local
            else:
                return EnvironmentTypes.click

    def _get_sample_destination_dir(self, env_type):
        if env_type == EnvironmentTypes.click:
            return os.path.expanduser("~/Pictures")
        else:
            return self._default_sample_destination_dir

    def configure_sample_files(self, env_type):
        self.sample_dir = self._sample_dirs[env_type]
        self.sample_destination_dir = self._get_sample_destination_dir(
            env_type
        )
        if (os.path.exists(self.sample_destination_dir)):
            shutil.rmtree(self.sample_destination_dir)
        self.assertFalse(os.path.exists(self.sample_destination_dir))

        self.sample_file = os.path.join(
            self.sample_destination_dir,
            "sample01.jpg"
        )

        default_data_dir = os.path.join(
            self.sample_dir,
            "default")
        shutil.copytree(default_data_dir, self.sample_destination_dir)
        self.assertTrue(os.path.isfile(self.sample_file))

        self.sample_file_source = \
            default_data_dir + self.sample_file_source

    def setUp(self):
        self.pointing_device = toolkit_emulators.get_pointing_device()
        super(GalleryTestCase, self).setUp()

        env_type = self._get_environment_launch_type()
        self.configure_sample_files(env_type)

        self.launch_gallery_app(env_type)

        self.addCleanup(shutil.rmtree, self.sample_destination_dir)

        """ This is needed to wait for the application to start.
        In the testfarm, the application may take some time to show up."""
        self.assertThat(self.gallery_utils.get_qml_view().visible,
                        Eventually(Equals(True)))
        """Wait for the data to be loaded and displayed"""
        self.ensure_at_least_one_event()
        """FIXME somehow on the server gallery sometimes is not fully started
        for switching to the albums view. Therefore this hack of a second"""
        sleep(1)

    def launch_gallery_app(self, env_type):
        if env_type == EnvironmentTypes.installed:
            self.launch_test_installed()
        elif env_type == EnvironmentTypes.local:
            self.launch_test_local()
        elif env_type == EnvironmentTypes.click:
            self.launch_test_click()
        else:
            raise ValueError("Unknown environment type: %s", env_type)

    def launch_test_local(self):
        logger.debug("Launching local gallery-app binary.")
        self.ARGS.append(self.sample_destination_dir)
        self.app = self.launch_test_application(
            self.local_location,
            *self.ARGS,
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    def launch_test_installed(self):
        if model() == 'Desktop':
            logger.debug(
                "Launching installed gallery-app binary for desktop."
            )
            self.ARGS.append(self.sample_destination_dir)
            self.app = self.launch_test_application(
                "gallery-app",
                *self.ARGS,
                emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)
        else:
            logger.debug(
                "Launching installed gallery-app binary for device."
            )
            self.ARGS.append("--desktop_file_hint="
                             "/usr/share/applications/gallery-app.desktop")
            self.ARGS.append(self.sample_destination_dir)
            self.app = self.launch_test_application(
                "gallery-app",
                *self.ARGS,
                app_type='qt',
                emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    def launch_test_click(self):
        '''
        Since this test runs under confinement, the only location photos
        are searchable in is ~/Pictures.
        '''
        logger.debug("Launching gallery-app via click package.")
        self.app = self.launch_click_package(
            package_id="com.ubuntu.gallery",
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    def ui_update(self):
        """ Gives the program the time to update the UI"""
        sleep(0.1)

    def click_item(self, item, delay=0.1):
        """Does a mouse click on the passed item, and moved the mouse there
           before"""
        # In jenkins test may fail because we don't wait before clicking the
        # target so we add a little delay before click.
        if model() == 'Desktop' and delay <= 0.25:
            delay = 0.25

        self.pointing_device.move_to_object(item)
        sleep(delay)
        self.pointing_device.click()

    def tap_item(self, item):
        """Does a long mouse press on the passed item, and moved the mouse
           there before"""
        self.pointing_device.move_to_object(item)
        self.pointing_device.press()
        sleep(1)
        self.pointing_device.release()

    def ensure_at_least_one_event(self):
        """The event view has to have at least one event
        In case gallery is not yet fully loaded wait a while and test again"""
        self.assertThat(lambda: self.gallery_utils.number_of_events(),
                        Eventually(GreaterThan(0)))

    def switch_to_albums_tab(self):
        self.main_view.switch_to_tab("albumsTab")

        albums_loader = self.gallery_utils.get_albums_viewer_loader()
        self.assertThat(albums_loader.progress, Eventually(Equals(1)))

        # The next check assumes that at least one album is available
        # Check if the albums are availabe - they need some time to load.
        self.assertThat(lambda: len(self.gallery_utils.get_all_albums()),
                        Eventually(GreaterThan(0)))
        self.ensure_tabs_dont_move()

    def ensure_tabs_dont_move(self):
        # FIXME find a (functional) way to test if the tabs still move
        sleep(1)

    def open_first_album(self):
        first_album = self.album_view.get_first_album()
        self.click_item(first_album)
        self.ensure_view_is_fully_open()

    def ensure_view_is_fully_open(self):
        animated_view = self.album_view.get_animated_album_view()
        self.assertThat(animated_view.isOpen, Eventually(Equals(True)))
        view = self.album_view.get_album_view()
        self.assertThat(view.visible, Eventually(Equals(True)))
        self.assertThat(animated_view.animationRunning,
                        Eventually(Equals(False)))

    def add_video_sample(self):
        video_file = "video20130618_0002.mp4"
        shutil.copyfile(self.sample_dir+"/option01/"+video_file,
                        self.sample_destination_dir+"/"+video_file)
        self.assertThat(
            lambda: self.gallery_utils.number_of_photos_in_events(),
            Eventually(Equals(3)))

    def get_delete_dialog(self):
        self.assertThat(lambda: self.gallery_utils.get_delete_dialog(),
                        Eventually(Not(Is(None))))
        delete_dialog = self.gallery_utils.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))
        self.assertThat(delete_dialog.opacity, Eventually(Equals(1)))
        return delete_dialog

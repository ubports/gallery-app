# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""gallery autopilot tests."""

import os
import logging
import shutil
import signal

from testtools.matchers import Equals, NotEquals, GreaterThan
from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from autopilot.introspection import get_proxy_object_for_existing_process
from pkg_resources import resource_filename

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

    _db = '~/.local/share/com.ubuntu.gallery/' \
          'database/gallery.sqlite'
    _thumbs = '~/.cache/com.ubuntu.gallery/thumbnails'

    _default_sample_destination_dir = "/tmp/gallery-ap_sd"

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
        if model() == 'Desktop':
            return EnvironmentTypes.installed
        else:
            if os.path.exists(self.local_location):
                return EnvironmentTypes.local
            else:
                return EnvironmentTypes.click

    def _get_sample_destination_dir(self, env_type):
        if env_type == EnvironmentTypes.click:
            pic_dir = os.path.expanduser("~/Pictures")
            pic_bak_dir = pic_dir + '.apbak'
            # Only save and restore if it previously existed
            if os.path.exists(pic_dir):
                shutil.move(pic_dir, pic_bak_dir)
                self.addCleanup(
                    logger.debug, "Restoring backed up pics to %s" % pic_dir)
                self.addCleanup(shutil.move, pic_bak_dir, pic_dir)
            return pic_dir
        else:
            return self._default_sample_destination_dir

    def configure_db(self):
        db = os.path.expanduser(self._db)
        db_bak = db + '.apbak'
        # Only save and restore if it previously existed
        if os.path.exists(db):
            shutil.move(db, db_bak)
            self.addCleanup(shutil.move, db_bak, db)
        if not os.path.exists(os.path.dirname(db)):
            os.makedirs(os.path.dirname(db))
        mock_db = os.path.join(self.sample_destination_dir, '.database',
                               'gallery_confined.sqlite')
        shutil.move(mock_db, db)

    def configure_thumbnails(self):
        thumbs = os.path.expanduser(self._thumbs)
        thumbs_bak = thumbs + '.apbak'
        # Only save and restore if it previously existed
        if os.path.exists(thumbs):
            shutil.move(thumbs, thumbs_bak)
            self.addCleanup(shutil.move, thumbs_bak, thumbs)
        if not os.path.exists(os.path.dirname(thumbs)):
            os.makedirs(os.path.dirname(thumbs))
        mock_thumbs = os.path.join(self.sample_destination_dir, '.thumbnails')
        shutil.move(mock_thumbs, thumbs)

    def configure_sample_files(self, env_type):
        self.sample_dir = resource_filename('gallery_app', 'data')
        self.sample_destination_dir = \
            self._get_sample_destination_dir(env_type)
        if (os.path.exists(self.sample_destination_dir)):
            shutil.rmtree(self.sample_destination_dir)
        self.assertFalse(os.path.exists(self.sample_destination_dir))

        self.sample_file = os.path.join(
            self.sample_destination_dir,
            "sample04.jpg"
        )

        default_data_dir = os.path.join(
            self.sample_dir,
            "default")
        shutil.copytree(default_data_dir, self.sample_destination_dir)
        self.assertTrue(os.path.isfile(self.sample_file))

        self.sample_file_source = \
            default_data_dir + self.sample_file_source

        if env_type == EnvironmentTypes.click:
            self.configure_db()
            self.configure_thumbnails()

    def do_reset_config(self):
        config = os.path.expanduser(
            os.path.join("~", ".config", "gallery-app.conf"))
        if os.path.exists(config):
            os.remove(config)

    def setUp(self):
        self.pointing_device = toolkit_emulators.get_pointing_device()
        super(GalleryTestCase, self).setUp()

        env_type = self._get_environment_launch_type()
        self.configure_sample_files(env_type)

        self.launch_gallery_app(env_type)

        self.addCleanup(shutil.rmtree, self.sample_destination_dir)
        self.addCleanup(logger.debug,
                        "Deleting %s" % self.sample_destination_dir)

        """ This is needed to wait for the application to start.
        In the testfarm, the application may take some time to show up."""
        qml_view = self.gallery_utils.get_qml_view()
        self.assertThat(qml_view.visible, Eventually(Equals(True)))
        """FIXME somehow on the server gallery sometimes is not fully started
        for switching to the albums view. Therefore this hack of sleeping"""
        sleep(2)

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
            app_uris=' '.join(self.ARGS),
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

    def open_album_at(self, position):
        album = self.album_view.get_album_at(position)
        self.click_item(album)
        self.ensure_view_is_fully_open()

    def open_first_album(self):
        self.open_album_at(0)

    def ensure_view_is_fully_open(self):
        view = self.album_view.get_album_view()
        self.assertThat(view.visible, Eventually(Equals(True)))

    def ensure_app_has_quit(self):
        """Terminate as gracefully as possible the application and ensure
        that it has fully quit before returning"""

        if model() == "Desktop":
            # On desktop to cleanly quit an app we just do the
            # equivalent of clicking on the close button in the window.
            self.keyboard.press_and_release("Alt+F4")
        else:
            # On unity8 at the moment we have no clean way to close the app.
            # So we ask the shell first to show the home, unfocusing our app,
            # which will save its state. Then we simply send it a SIGTERM to
            # force it to quit.
            # See bug https://bugs.launchpad.net/unity8/+bug/1261720 for more
            # details.
            from unity8 import process_helpers
            pid = process_helpers._get_unity_pid()
            unity8 = get_proxy_object_for_existing_process(pid=pid)
            shell = unity8.select_single("Shell")
            shell.slots.showHome()
            self.assertThat(shell.focusedApplicationId,
                            Eventually(NotEquals("gallery-app")))
            self.app.process.send_signal(signal.SIGTERM)

        # Either way, we wait for the underlying process to be fully finished.
        self.app.process.wait()
        self.assertIsNotNone(self.app.process.returncode)

    def get_delete_dialog(self):
        """Raises StateNotFoundError if get_delete_dialog fails."""
        delete_dialog = self.gallery_utils.get_delete_dialog()
        self.assertThat(delete_dialog.visible, Eventually(Equals(True)))
        self.assertThat(delete_dialog.opacity, Eventually(Equals(1)))
        return delete_dialog

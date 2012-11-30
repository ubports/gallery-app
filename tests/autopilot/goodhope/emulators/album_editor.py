# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2012 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.


class AlbumEditor(object):
    """An emulator class that makes it easy to interact with the gallery app."""

    def __init__(self, app):
        self.app = app

    def get_qml_view(self):
        """Get the main QML view"""
        return self.app.select_single("QQuickView")

    def get_albums_tab(self):
        """Returns the 'Albums' tab."""
        return self.app.select_single("Tab", objectName="toolbarAlbumsTab")

    def get_plus_icon(self):
        """Returns the 'plus' icon of the main view."""
        return self.app.select_single("StandardToolbarIconButton", objectName="toolbarPlusIcon")

    def get_album_editor(self):
        """Returns the album editor."""
        return self.app.select_single("AlbumEditor", objectName="mainAlbumEditor")

    def get_album_title_entry_field(self):
        """Returns the album title input box."""
        aa_qqr = self.get_album_editor().get_children_by_type("AspectArea")[0].get_children_by_type("QQuickItem")[0]
        ac_qqi = aa_qqr.get_children_by_type("AlbumCover")[0].get_children_by_type("QQuickImage")[0]
        return ac_qqi.get_children_by_type("QQuickColumn")[0].get_children_by_type("TextEditOnClick")[0]

    def get_album_subtitle_entry_field(self):
        """Returns the album subtitle input box."""
        aa_qqr = self.get_album_editor().get_children_by_type("AspectArea")[0].get_children_by_type("QQuickItem")[0]
        ac_qqi = aa_qqr.get_children_by_type("AlbumCover")[0].get_children_by_type("QQuickImage")[0]
        return ac_qqi.get_children_by_type("QQuickColumn")[0].get_children_by_type("TextEditOnClick")[1]
/*
 * Copyright (C) 2014-2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Extras 0.2 as Extras

Page {
    id: page
    property string photo
    signal done(bool photoWasModified)
    
    title: i18n.tr("Edit Photo")
    
    head.backAction: Action {
        iconName: "save"
        onTriggered: editor.close(true)
    }
    head.actions: editor.actions

    Extras.PhotoEditor {
        id: editor
        anchors.fill: parent
        onClosed: page.done(photoWasModified)
    }

    onActiveChanged: {
        if (active) editor.open(page.photo)
    }
}

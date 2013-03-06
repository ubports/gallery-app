/*
 * Copyright (C) 2012 Canonical Ltd
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
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import "../Utility"
import "../../Capetown/Widgets"

/*!
*/
PopupMenu {
    id: selectionMenu

    /*!
    */
    property SelectionState selection

    // these are hardcoded to match the location of the Selection Button in the
    // chrome
    popupOriginX: units.gu(3.5)
    popupOriginY: -units.gu(6)

    visible: false
    state: "hidden"

    onActionInvoked: {
        // See https://bugreports.qt-project.org/browse/QTBUG-17012 before you edit
        // a switch statement in QML.  The short version is: use braces always.
        switch (name) {
        case "SelectAll": {
            if (selection)
                selection.selectAll();
            break;
        }

        case "SelectNone": {
            if (selection)
                selection.unselectAll();
            break;
        }

        default: {
            console.log("Unknown action", name);
            break;
        }
        }
    }

    model: ListModel {
        ListElement {
            title: "Select All"
            action: "SelectAll"
            hasBottomBorder: true
        }

        ListElement {
            title: "Select None"
            action: "SelectNone"
        }
    }
}

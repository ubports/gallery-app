/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 1.1
import "../../Capetown/Widgets"

PopupMenu {
  model: ListModel {
    ListElement {
      title: "Share"
      isSeparator: true
    }

    ListElement {
      title: "Quick Share"
      hasBottomBorder: true
    }

    ListElement {
      title: "Create Link"
    }

    ListElement {
      title: "Paste into app"
      isSeparator: true
    }

    ListElement {
      title: "GMail"
      hasBottomBorder: true
      iconFilename: "../../img/dummy-menu-icon.png"
    }

    ListElement {
      title: "Yahoo! Mail"
      iconFilename: "../../img/dummy-menu-icon.png"
    }
  }
}

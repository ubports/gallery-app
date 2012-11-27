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

import QtQuick 2.0

PopupBox {
  id: popupMenu

  // public
  property alias model: menuItemList.model

  signal actionInvoked(string name);

  width: units.gu(40)
  height: menuItemColumn.height + originCueHeight

  color: "transparent"

  Column {
    id: menuItemColumn

    Repeater {
      id: menuItemList

      x: popupMenu.contentLeft
      y: popupMenu.contentTop
      width: popupMenu.width

      delegate: MenuItem {
        title: (model.title) ? model.title : ""
        isSeparator: (model.isSeparator) ? model.isSeparator : false
        hasBottomBorder: (model.hasBottomBorder) ? model.hasBottomBorder : false
        iconFilename: (model.iconFilename) ? model.iconFilename : ""
        hasCueRectangle: (model.hasCueRectangle) ? model.hasCueRectangle : false
        action: (model.action) ? model.action : ""
        hostMenu: popupMenu

        onActionInvoked: {
          popupMenu.actionInvoked(name);
        }

        onPopupInteractionCompleted: {
          popupMenu.popupInteractionCompleted();
        }
      }
    }
  }

  Rectangle {
    id: overstroke

    width: parent.width
    height: parent.height - parent.originCueHeight

    color: "transparent"

    border.color: "#a7a9ac"
    border.width: 1
  }
}

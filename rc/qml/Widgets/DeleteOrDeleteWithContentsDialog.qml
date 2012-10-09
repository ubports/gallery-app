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
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 2.0
import "../../Capetown/Widgets"

PopupActionChoiceDialog {
  id: deleteOrDeleteWithContentsDialog
  
  property variant album
  property alias deleteTitle: deleteOrDeleteWithContentsDialog.action0Title
  property alias deleteWithContentsTitle: deleteOrDeleteWithContentsDialog.action1Title
  
  signal deleteRequested();
  signal deleteWithContentsRequested();
  
  visible: false
  
  action0Title: "Delete"
  onAction0Requested: deleteRequested()

  action1Title: "Delete with contents"
  onAction1Requested: deleteWithContentsRequested()
}

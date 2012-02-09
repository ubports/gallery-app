/*
 * Copyright (C) 2011 Canonical Ltd
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

import QtQuick 1.1

Rectangle {
  property variant mediaSource
  property int gutter: 0
  property bool isPreview: false
  
  PhotoComponent {
    id: photo
    objectName: "photo"
    
    anchors.centerIn: parent
    
    width: parent.width - (gutter * 2)
    height: parent.height - (gutter * 2)
    
    mediaSource: parent.mediaSource
    ownerName: "FramePortrait"
    
    isCropped: true
    isPreview: parent.isPreview
  }
}

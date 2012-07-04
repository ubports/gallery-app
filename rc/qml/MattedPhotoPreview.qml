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
 * Charles Lindsay <chaz@yorba.org>
 */

import QtQuick 1.1
import Gallery 1.0

MattedPhoto {
  property bool isSelected
  
  isPreview: true

  backingImage: isSelected ? "../img/photo-preview-backing-selected.png"
    : "../img/photo-preview-backing.png"
  insideShadowImage: "../img/photo-preview-inside-shadow.png"
  overlayImage: isSelected ? "../img/photo-preview-selected-overlay.png" : ""
  
  mattedPhotoWidth: gu(24)
  mattedPhotoHeight: gu(18)
  backingStartX: 2
  backingStartY: 2
}

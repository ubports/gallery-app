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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Eric Gregory <eric@yorba.org>
 */

import QtQuick 1.1
import "../Capetown/Viewer"

// The GalleryPhotoComponent adds a MediaSource object to a PhotoComponent.
// The PhotoComponent's source is updated automatically from the MediaSource.
PhotoComponent {
  id: galleryPhotoComponent
  
  property variant mediaSource
  
  source: {
    if (!mediaSource)
      return "";
    
    // TODO: If MediaSource could return dimensions of both original image
    // and its preview, could dynamically determine if the display dimensions
    // are smaller than preview's and automatically use that instead of a
    // full image load
    
    // Load image using the Gallery image provider to ensure EXIF orientation
    return isPreview ? mediaSource.galleryPreviewPath : mediaSource.galleryPath
  }
}

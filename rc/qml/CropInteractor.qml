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
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0
import Gallery 1.0
import Ubuntu.Components 0.1
import "../Capetown"
import "../Capetown/Viewer"
import "Components"
import "Widgets"
import "Utility"
import "../js/GraphicsRoutines.js" as GraphicsRoutines

Item {
  id: cropInteractor

  property string matteColor: "black"
  property real matteOpacity: 0.6

  // Note: each element of the cropped rect will be in the range [0,1], since
  // in the UI we aren't using direct photo pixel values.
  signal cropped(variant rect)
  signal canceled()

  // readonly
  property variant photo // Set with enter().

  // internal
  property variant ratio_crop_rect

  function computeRectSet(photo, relativeCropRect) {
    var result = { };

    var wholePhotoPreviewRect = GraphicsRoutines.fitRect(viewport,
      cropInteractor.photo);

    var unfitCropRect = { };
    unfitCropRect.x = relativeCropRect.x * wholePhotoPreviewRect.width;
    unfitCropRect.y = relativeCropRect.y * wholePhotoPreviewRect.height;
    unfitCropRect.width = relativeCropRect.width * wholePhotoPreviewRect.width;
    unfitCropRect.height = relativeCropRect.height *
      wholePhotoPreviewRect.height;

    var cropFrameRect = GraphicsRoutines.fitRect(viewport, unfitCropRect);

    var photoExtentRect = { };
    photoExtentRect.x = cropFrameRect.x - (cropFrameRect.scaleFactor *
      wholePhotoPreviewRect.width * relativeCropRect.x);
    photoExtentRect.y = cropFrameRect.y - (cropFrameRect.scaleFactor *
      wholePhotoPreviewRect.height * relativeCropRect.y);
    photoExtentRect.width = cropFrameRect.scaleFactor *
      wholePhotoPreviewRect.width;
    photoExtentRect.height = cropFrameRect.scaleFactor *
      wholePhotoPreviewRect.height;
    photoExtentRect.scaleFactor = cropFrameRect.scaleFactor;

    result.photoPreviewRect = wholePhotoPreviewRect;
    result.cropFrameRect = cropFrameRect;
    result.photoExtentRect = photoExtentRect;

    return result;
  }

  function enter(photo, ratio_crop_rect) {
    cropInteractor.photo = photo;
    original.mediaSource = photo;
    cropInteractor.ratio_crop_rect = ratio_crop_rect;

    var rects = computeRectSet(photo, ratio_crop_rect);

    overlay.initialFrameX = rects.cropFrameRect.x;
    overlay.initialFrameY = rects.cropFrameRect.y;
    overlay.initialFrameWidth = rects.cropFrameRect.width;
    overlay.initialFrameHeight = rects.cropFrameRect.height;

    overlay.resetFor(rects);

    original.visible = true;
    overlay.visible = true;
  }

  Item {
    id: viewport

    anchors.fill: parent
    anchors.margins: units.gu(8)
    z: 1
  }

  CropOverlay {
    id: overlay

    property real minSize: units.gu(4)

    anchors.fill: parent;
    visible: false;

    photo: original
    viewport: viewport

    matteColor: cropInteractor.matteColor
    matteOpacity: cropInteractor.matteOpacity

    z: 16

    onMatteRegionPressed: {
      cropInteractor.canceled();
    }

    onCropButtonPressed: {
      original.visible = false;
      overlay.visible = false;
      original.scale = 1.0;
      cropInteractor.cropped(overlay.getRelativeFrameRect());
    }
  }

  GalleryPhotoComponent {
    id: original

    x: viewport.x;
    y: viewport.y;
    width: viewport.width;
    height: viewport.height;
    transformOrigin: Item.TopLeft;

    color: "black"
    visible: false
    load: true

    mediaSource: cropInteractor.photo
    ownerName: "cropInteractor"

    onLoaded: {
      var rects = cropInteractor.computeRectSet(cropInteractor.photo,
        cropInteractor.ratio_crop_rect);

      x = rects.photoExtentRect.x;
      y = rects.photoExtentRect.y;
      width = rects.photoPreviewRect.width;
      height = rects.photoPreviewRect.height;
      scale = rects.photoExtentRect.scaleFactor;
    }
  }
}

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

import QtQuick 1.1
import Gallery 1.0
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
  property variant photo // Set with enterCropper().

  // internal
  property variant ratio_crop_rect

  function enterCropper(photo, ratio_crop_rect) {
    cropInteractor.photo = photo;
    original.mediaSource = photo;
    cropInteractor.ratio_crop_rect = ratio_crop_rect;

    var fitRect = GraphicsRoutines.fitRect(viewport, cropInteractor.photo);

    overlay.initialFrameX = fitRect.x;
    overlay.initialFrameY = fitRect.y;
    overlay.initialFrameWidth = fitRect.width;
    overlay.initialFrameHeight = fitRect.height;

    overlay.reset();
    overlay.visible = true;
    original.visible = true;
  }

  Item {
    id: viewport

    anchors.fill: parent
    anchors.margins: gu(8)
    z: 1
  }

  CropOverlay {
    id: overlay

    property real minSize: gu(4)

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
      cropInteractor.cropped(overlay.getRelativeFrameRect());
    }
  }

  GalleryPhotoComponent {
    id: original

    x: viewport.x;
    y: viewport.y;
    width: viewport.width;
    height: viewport.height;

    color: "black"
    visible: false

    mediaSource: cropInteractor.photo
    ownerName: "cropInteractor"

    onLoaded: {
      var fitRect = GraphicsRoutines.fitRect(viewport, cropInteractor.photo);

      x = fitRect.x;
      y = fitRect.y;
      width = fitRect.width;
      height = fitRect.height;

      overlay.initialFrameX = x;
      overlay.initialFrameY = y;
      overlay.initialFrameWidth = width;
      overlay.initialFrameHeight = height;
    }
  }
}

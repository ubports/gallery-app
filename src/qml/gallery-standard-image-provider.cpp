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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "qml/gallery-standard-image-provider.h"

#include <QFileInfo>

#include <climits>
#include <memory>

#include "photo/photo-metadata.h"

GalleryStandardImageProvider* GalleryStandardImageProvider::instance_ = NULL;

GalleryStandardImageProvider::GalleryStandardImageProvider()
  : QDeclarativeImageProvider(QDeclarativeImageProvider::Image) {
}

GalleryStandardImageProvider::~GalleryStandardImageProvider() {
}

GalleryStandardImageProvider* GalleryStandardImageProvider::instance() {
  if (!instance_) {
    instance_ = new GalleryStandardImageProvider();
  }

  return instance_;
}

QImage GalleryStandardImageProvider::requestImage(const QString& id,
  QSize* size, const QSize& requestedSize) {
  std::auto_ptr<PhotoMetadata> metadata(PhotoMetadata::FromFile(id));
  QImage loaded = QImage(id);

  // respect EXIF orientation in photo metadata, if any
  if (metadata.get() != NULL && metadata->orientation() != TOP_LEFT_ORIGIN)
    loaded = loaded.transformed(metadata->orientation_transform());

  double aspect_ratio = ((double) loaded.width()) / ((double) loaded.height());

  if (requestedSize.width() > 0 && requestedSize.height() > 0) {
    // if both width and height were explicitly specified, constrain on height
    // if portrait and constrain on width if landscape
    if (aspect_ratio < 1.0)
      loaded = loaded.scaledToHeight(requestedSize.height(),
        Qt::SmoothTransformation);
    else
      loaded = loaded.scaledToWidth(requestedSize.width(),
        Qt::SmoothTransformation);
  } else if (requestedSize.width() > 0) {
    // if only width was specified, constrain on width
      loaded = loaded.scaledToWidth(requestedSize.width(),
        Qt::SmoothTransformation);
  } else if (requestedSize.height() > 0) {
    // if only height was specified, constrain on height
      loaded = loaded.scaledToHeight(requestedSize.height(),
        Qt::SmoothTransformation);
  } else {
      // if neither width nor height specified, do nothing -- the QML layer is
      // getting back a full-sized image
      ;
  }

  size->setWidth(loaded.width());
  size->setHeight(loaded.height());

  return loaded;
}

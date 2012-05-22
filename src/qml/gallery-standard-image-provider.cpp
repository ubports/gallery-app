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
 * Jim Nelson <jim@yorba.org>
 */

#include "qml/gallery-standard-image-provider.h"

#include <QImageReader>
#include <QSize>

#include "photo/photo-metadata.h"

GalleryStandardImageProvider* GalleryStandardImageProvider::instance_ = NULL;

const char* GalleryStandardImageProvider::PROVIDER_ID = "gallery-standard";
const char* GalleryStandardImageProvider::PROVIDER_ID_SCHEME = "image://gallery-standard/";

GalleryStandardImageProvider::GalleryStandardImageProvider()
  : QDeclarativeImageProvider(QDeclarativeImageProvider::Image) {
}

GalleryStandardImageProvider::~GalleryStandardImageProvider() {
}

void GalleryStandardImageProvider::Init() {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new GalleryStandardImageProvider();
}

GalleryStandardImageProvider* GalleryStandardImageProvider::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

QUrl GalleryStandardImageProvider::ToURL(const QFileInfo& file) {
  return QUrl::fromUserInput(PROVIDER_ID_SCHEME + file.absoluteFilePath());
}

QImage GalleryStandardImageProvider::requestImage(const QString& id,
  QSize* size, const QSize& requestedSize) {
  // Note that even though id looks like a path at this point, it can contain
  // arbitrary "parameters" that came from the original URL.  We use these
  // parameters to ensure that we reload images from disk (skipping QML's
  // internal cache) when editing photos.  So, for now, we strip off everything
  // but the path part of the "URL" (the ?edit=x part is simply ignored).
  QUrl url(id);
  QString file = url.path();

  QImageReader reader(file);
  
  // use scaled load-and-decode if size has been requested
  if (requestedSize.width() > 0 || requestedSize.height() > 0) {
    // load file's original size
    QSize originalSize = reader.size();
    
    // only scale if scaling down, not up
    if (originalSize.isValid() && originalSize.width() >= requestedSize.width()
      && originalSize.height() >= requestedSize.height()) {
      // scale by aspect ratio, expanding if necessary ... note that the scale()
      // method will deal with one zero or negative dimension (but obviously not
      // with two)
      originalSize.scale(requestedSize, Qt::KeepAspectRatioByExpanding);
      
      // configure reader for scaled load-and-decode
      reader.setScaledSize(originalSize);
    }
  }
  
  QImage image = reader.read();
  if (image.isNull()) {
    qDebug("Unable to load %s: %s", qPrintable(id), qPrintable(reader.errorString()));
    
    return image;
  }
  
  // apply orientation if necessary
  std::auto_ptr<PhotoMetadata> metadata(PhotoMetadata::FromFile(file));
  if (metadata.get() != NULL && metadata->orientation() != TOP_LEFT_ORIGIN)
    image = image.transformed(metadata->orientation_transform());
  
  if (size != NULL)
    *size = image.size();
  
#ifdef GALLERY_LOG_IMAGE
  qDebug("Loaded %s req:%dx%d ret:%dx%d", qPrintable(id), requestedSize.width(),
    requestedSize.height(), image.width(), image.height());
#endif
  
  return image;
}

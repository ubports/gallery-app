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
 * Guenter Schwann <guenter.schwann@canonical.com>
*/

#include "gallery-thumbnail-image-provider.h"
#include "gallery-application.h"

#include <QDebug>
#include <QElapsedTimer>

GalleryThumbnailImageProvider* GalleryThumbnailImageProvider::instance_ = NULL;

const char* GalleryThumbnailImageProvider::PROVIDER_ID = "gallery-thumbnail";
const char* GalleryThumbnailImageProvider::PROVIDER_ID_SCHEME = "image://gallery-thumbnail/";

const char* GalleryThumbnailImageProvider::REVISION_PARAM_NAME = "edit";

/*!
 * @brief GalleryThumbnailImageProvider::GalleryThumbnailImageProvider
 */
GalleryThumbnailImageProvider::GalleryThumbnailImageProvider()
  : QQuickImageProvider(QQuickImageProvider::Image) {
}

/*!
 * @brief GalleryThumbnailImageProvider::Init needs to be called once befoe any other method of this
 * class
 */
void GalleryThumbnailImageProvider::Init() {
  Q_ASSERT(instance_ == NULL);
  instance_ = new GalleryThumbnailImageProvider();
}

/*!
 * @brief GalleryThumbnailImageProvider::instance returns the on instanc that exists in the app
 * @return instance of this class to use
 */
GalleryThumbnailImageProvider* GalleryThumbnailImageProvider::instance() {
  Q_ASSERT(instance_ != NULL);
  return instance_;
}

/*!
 * \brief GalleryThumbnailImageProvider::ToURL return the URL to this image provider for a local file
 * \param file is the file that you want the url for
 * \return URL to be used to load a local image usign this image provider
 */
QUrl GalleryThumbnailImageProvider::ToURL(const QFileInfo &file) {
  return QUrl::fromUserInput(PROVIDER_ID_SCHEME + file.absoluteFilePath());
}

/*!
 * @brief GalleryThumbnailImageProvider::requestImage @reimp of QQuickImageProvider::requestImage
 * @param id is the requested image source
 * @param size must be set to the original size of the image
 * @param requestedSize is not used here, as thumbnails have a fixed size
 * @return the image with id
 */
QImage GalleryThumbnailImageProvider::requestImage(const QString &id, QSize *size,
  const QSize &requestedSize)
{
  Q_UNUSED(requestedSize);
  QElapsedTimer timer;
  timer.start();

  QUrl url(id);
  QString fileName = url.path();

  QImage thumbnail;
  thumbnail.load(fileName);
  if (thumbnail.isNull()) {
      qWarning() << "Could not load thumbnail:" << id;
  }

  if (size != NULL)
    *size = thumbnail.size();

  if (GalleryApplication::instance()->log_image_loading()) {
      qDebug() << id << thumbnail.size() << "time:" << timer.elapsed() << "ms";
  }

  return thumbnail;
}

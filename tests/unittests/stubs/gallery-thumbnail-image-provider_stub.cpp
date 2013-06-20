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

const char* GalleryThumbnailImageProvider::PROVIDER_ID = "gallery-thumbnail";
const char* GalleryThumbnailImageProvider::PROVIDER_ID_SCHEME = "image://gallery-thumbnail/";
const char* GalleryThumbnailImageProvider::REVISION_PARAM_NAME = "edit";

GalleryThumbnailImageProvider::GalleryThumbnailImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_previewManager(0),
      m_logImageLoading(false)
{
}

QImage GalleryThumbnailImageProvider::requestImage(const QString& id,
                                                  QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);
    return QImage();
}

QUrl GalleryThumbnailImageProvider::toURL(const QFileInfo &file)
{
    return QUrl::fromUserInput(PROVIDER_ID_SCHEME + file.absoluteFilePath());
}

void GalleryThumbnailImageProvider::setPreviewManager(PreviewManager *previewManager)
{
    m_previewManager = previewManager;
}

void GalleryThumbnailImageProvider::setLogging(bool enableLogging)
{
    m_logImageLoading = enableLogging;
}

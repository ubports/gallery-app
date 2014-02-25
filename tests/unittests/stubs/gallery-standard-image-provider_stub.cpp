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

#include "gallery-standard-image-provider.h"

const char* GalleryStandardImageProvider::PROVIDER_ID = "gallery-standard";
const char* GalleryStandardImageProvider::PROVIDER_ID_SCHEME = "image://gallery-standard/";

const char* GalleryStandardImageProvider::REVISION_PARAM_NAME = "edit";
const char* GalleryStandardImageProvider::ORIENTATION_PARAM_NAME = "orientation";

const char* GalleryStandardImageProvider::SIZE_KEY = "size_level";

const long MAX_CACHE_BYTES = 20L * 1024L * 1024L;

// fully load previews into memory when requested
const int SCALED_LOAD_FLOOR_DIM_PIXELS = 360;

GalleryStandardImageProvider::GalleryStandardImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_cachedBytes(0),
      m_logImageLoading(false),
      m_maxLoadResolution(INT_MAX)
{
}

GalleryStandardImageProvider::~GalleryStandardImageProvider()
{
}

QImage GalleryStandardImageProvider::requestImage(const QString& id,
                                                  QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);
    return QImage();
}

QUrl GalleryStandardImageProvider::toURL(const QFileInfo& file)
{
    return QUrl::fromUserInput(PROVIDER_ID_SCHEME + file.absoluteFilePath());
}

void GalleryStandardImageProvider::setLogging(bool enableLogging)
{
    m_logImageLoading = enableLogging;
}

void GalleryStandardImageProvider::setMaxLoadResolution(int resolution)
{
    m_maxLoadResolution = resolution;
}

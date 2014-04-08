/*
 * Copyright (C) 2013 Canonical Ltd
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
 */

#include "video.h"

// util
#include <resource.h>

#include <QUrl>

Video::Video(const QFileInfo &file)
    :MediaSource(file)
{
    Q_UNUSED(file);
}

MediaSource::MediaType Video::type() const
{
    return MediaSource::Video;
}

QImage Video::image(bool respectOrientation, const QSize &scaleSize)
{
    Q_UNUSED(respectOrientation);
    Q_UNUSED(scaleSize);
    return QImage();
}

bool Video::isCameraVideo(const QFileInfo &file)
{
    QMimeDatabase mimedb;
    QMimeType mimeType = mimedb.mimeTypeForFile(file);
    qWarning() << mimeType.name();
    if (mimeType.name().contains("video")) {
        return true;
    }

    return false;
}

bool Video::isValid(const QFileInfo &file)
{
    Q_UNUSED(file);

    return true;
}

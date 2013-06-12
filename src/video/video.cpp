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

/*!
 * \brief Video::Video
 * \param file
 */
Video::Video(const QFileInfo &file)
    :MediaSource(file)
{
}

/*!
 * \reimp
 */
MediaSource::MediaType Video::type() const
{
    return MediaSource::Video;
}

/*!
 * \reimp
 */
QImage Video::image(bool respect_orientation, const QSize &scaleSize)
{
    Q_UNUSED(respect_orientation);
    Q_UNUSED(scaleSize);
    return QImage(Resource::getRcUrl("img/video-thumbnail.png").toLocalFile());
}

/*!
 * \reimp
 */
QUrl Video::galleryPath() const
{
    return Resource::getRcUrl("img/video-thumbnail.png");
}

/*!
 * \reimp
 */
QUrl Video::galleryPreviewPath() const
{
    return Resource::getRcUrl("img/video-thumbnail.png");
}

/*!
 * \reimp
 */
QUrl Video::galleryThumbnailPath() const
{
    return Resource::getRcUrl("img/video-thumbnail.png");
}

/*!
 * \brief Video::isCameraVideo return true if the video file was created by the
 * camera-app
 * \param file the file to check
 * \return
 */
bool Video::isCameraVideo(const QFileInfo &file)
{
    if (file.suffix() == QLatin1String("mp4") &&
            file.baseName().startsWith("video")) {
        return true;
    }

    return false;
}

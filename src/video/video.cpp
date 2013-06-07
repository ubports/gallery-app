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
QImage Video::image(bool respect_orientation, const QSize &scaleSize)
{
    Q_UNUSED(respect_orientation);
    Q_UNUSED(scaleSize);
    return QImage(Resource::get_rc_url("img/video-thumbnail.png").toLocalFile());
}

QUrl Video::galleryPath() const
{
    return Resource::get_rc_url("img/video-thumbnail.png");
}

QUrl Video::galleryPreviewPath() const
{
    return Resource::get_rc_url("img/video-thumbnail.png");
}

QUrl Video::galleryThumbnailPath() const
{
    return Resource::get_rc_url("img/video-thumbnail.png");
}

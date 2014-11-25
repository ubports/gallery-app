/*
 * Copyright (C) 2011-2012 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 * Lucas Beeler <lucas@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 * Eric Gregory <eric@yorba.org>
 * Clint Rogers <clinton@yorba.org>
 */

#include "photo.h"

// util
#include <orientation.h>

#include <QFileInfo>
#include <QImage>

QFileInfo photoDummyFileInfo;

bool Photo::isValid(const QFileInfo& file)
{
    Q_UNUSED(file);
    return true;
}

/*!
 * \brief Photo::Photo
 * \param file
 */
Photo::Photo(const QFileInfo& file)
    : MediaSource(file),
      m_editRevision(0),
      m_caches(file),
      m_originalSize(),
      m_originalOrientation(TOP_LEFT_ORIGIN)
{
    photoDummyFileInfo = file;
}

Photo::~Photo()
{
}

MediaSource::MediaType Photo::type() const
{
    return MediaSource::Photo;
}

QImage Photo::image(bool respectOrientation, const QSize &scaleSize)
{
    return QImage();
}

Orientation Photo::orientation() const
{
    return m_originalOrientation;
}

QUrl Photo::galleryPath() const
{
    QUrl url = MediaSource::galleryPath();
    return url;
}

QUrl Photo::galleryPreviewPath() const
{
    QUrl url = MediaSource::galleryPreviewPath();
    return url;
}

QUrl Photo::galleryThumbnailPath() const
{
    QUrl url = MediaSource::galleryThumbnailPath();
    return url;
}

const QFileInfo &Photo::pristineFile() const
{
    return photoDummyFileInfo;
}

void Photo::destroySource(bool destroyBacking, bool asOrphan)
{
}

void Photo::setOriginalOrientation(Orientation orientation)
{
    m_originalOrientation = orientation;
}

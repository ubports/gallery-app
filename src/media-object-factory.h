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

#ifndef MEDIA_OBJECT_FACTORY_H_
#define MEDIA_OBJECT_FACTORY_H_

// media
#include "media-source.h"

// utils
#include <orientation.h>

#include <QDateTime>
#include <QFileInfo>
#include <QSize>

class MediaTable;

/*!
 * \brief The MediaObjectFactory creates phot and video objects
 */
class MediaObjectFactory
{
public:
    explicit MediaObjectFactory();

    void setMediaTable(MediaTable *mediaTable);
    void enableContentLoadFilter(MediaSource::MediaType filterType);

    MediaSource *create(const QFileInfo& file);

private:
    void clearMetadata();
    bool readPhotoMetadata(const QFileInfo &file);
    bool readVideoMetadata(const QFileInfo &file);

    MediaTable *m_mediaTable;
    QDateTime m_timeStamp;
    QDateTime m_exposureTime;
    QSize m_size;
    Orientation m_orientation;
    qint64 m_fileSize;

    MediaSource::MediaType m_filterType;

    friend class tst_MediaObjectFactory;
};

#endif  // MEDIA_OBJECT_FACTORY_H_

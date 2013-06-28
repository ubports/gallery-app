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

#include "media-object-factory.h"

// database
#include "database.h"
#include "media-table.h"

// medialoader
#include "photo-metadata.h"

// photo
#include "photo.h"

// video
#include <video.h>

/*!
 * \brief MediaObjectFactory::MediaObjectFactory
 * \param mediaTable
 */
MediaObjectFactory::MediaObjectFactory()
    : m_mediaTable(),
      m_filterType(MediaSource::None)
{
}

/*!
 * \brief MediaObjectFactory::setMediaTable
 * \param mediaTable
 */
void MediaObjectFactory::setMediaTable(MediaTable *mediaTable)
{
    m_mediaTable = mediaTable;
}

/*!
 * \brief GalleryManager::enableContentLoadFilter enable filter to load only
 * content of certain type
 * \param filterType
 */
void MediaObjectFactory::enableContentLoadFilter(MediaSource::MediaType filterType)
{
    m_filterType = filterType;
}

/*!
 * \brief MediaObjectFactory::create loads the data for a photo or video file.
 * Creates / updates the database as well.
 * \param file the file to load
 * \return 0 if this no valid photo/video file
 */
MediaSource *MediaObjectFactory::create(const QFileInfo &file)
{
    Q_ASSERT(m_mediaTable);

    clearMetadata();

    MediaSource::MediaType mediaType = MediaSource::Photo;
    if (Video::isCameraVideo(file))
        mediaType = MediaSource::Video;

    if (m_filterType != MediaSource::None && mediaType != m_filterType)
        return 0;

    // Look for video in the database.
    qint64 id = m_mediaTable->getIdForMedia(file.absoluteFilePath());

    if (id == INVALID_ID) {
        if (mediaType == MediaSource::Video && (!file.exists()))
            return 0;
        if (mediaType == MediaSource::Photo && !Photo::isValid(file))
            return 0;
    }

    MediaSource *media = 0;
    Photo *photo = 0;
    Video *video = 0;
    if (mediaType == MediaSource::Photo) {
        photo = new Photo(file);
        media = photo;
    } else {
        video = new Video(file);
        media = video;
    }

    if (id == INVALID_ID) {
        bool metadataOk;
        if (mediaType == MediaSource::Photo) {
            metadataOk = readPhotoMetadata(photo->pristineFile());
        } else {
            metadataOk = readVideoMetadata(file);
        }

        if (!metadataOk) {
            delete media;
            return 0;
        }

        // Add to DB.
        id = m_mediaTable->createIdForMedia(file.absoluteFilePath(), m_timeStamp,
                                            m_exposureTime, m_orientation, m_fileSize);
    } else {
        // Load metadata from DB.
        m_mediaTable->getRow(id, m_size, m_orientation, m_timeStamp, m_exposureTime);
    }
    media->setSize(m_size);
    media->setFileTimestamp(m_timeStamp);
    media->setExposureDateTime(m_exposureTime);
    if (mediaType == MediaSource::Photo)
        photo->setOriginalOrientation(m_orientation);
    media->setId(id);

    return media;
}

/*!
 * \brief MediaObjectFactory::clearMetadata resets all memeber variables
 * regarding metadata
 */
void MediaObjectFactory::clearMetadata()
{
    m_timeStamp = QDateTime();
    m_orientation = TOP_LEFT_ORIGIN;
    m_fileSize = 0;
    m_exposureTime = QDateTime();
    m_size = QSize();
}

/*!
 * \brief MediaObjectFactory::readPhotoMetadata
 * \param file
 * \return 0 if there was an error reading the metadata
 */
bool MediaObjectFactory::readPhotoMetadata(const QFileInfo &file)
{
    PhotoMetadata* metadata = PhotoMetadata::fromFile(file);
    if (metadata == 0)
        return false;

    m_timeStamp = file.lastModified();
    m_orientation = metadata->orientation();
    m_fileSize = file.size();
    m_exposureTime = metadata->exposureTime().isValid() ?
                QDateTime(metadata->exposureTime()) : m_timeStamp;
    m_size = QSize();

    return true;
}

/*!
 * \brief MediaObjectFactory::readVideoMetadata
 * \param file
 * \return 0 if there was an error reading the metadata
 */
bool MediaObjectFactory::readVideoMetadata(const QFileInfo &file)
{
    if (!file.exists())
        return false;

    m_timeStamp = file.created();
    m_orientation = TOP_LEFT_ORIGIN;
    m_fileSize = file.size();
    m_exposureTime = file.created();
    m_size = QSize(256, 256);

    return true;
}

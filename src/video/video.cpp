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

// database
#include "database.h"
#include "media-table.h"

// src
#include "gallery-manager.h"

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
 * \brief Video::load
 * \param file
 * \return
 */
Video *Video::load(const QFileInfo &file)
{
    bool needs_update = false;
    QDateTime timestamp;
    QDateTime exposureTime;
    QSize size;
    Orientation orientation;
    qint64 fileSize;

    // Look for video in the database.
    MediaTable *mediaTable = GalleryManager::instance()->database()->getMediaTable();
    qint64 id = GalleryManager::instance()->database()->getMediaTable()->getIdForMedia(
                file.absoluteFilePath());

    if (id == INVALID_ID && (!file.exists() || !isCameraVideo(file)))
        return 0;

    Video *video = new Video(file);

    if (id != INVALID_ID)
        needs_update = mediaTable->rowNeedsUpdate(id);

    if (id == INVALID_ID || needs_update) {
        timestamp = file.created();
        orientation = TOP_LEFT_ORIGIN;
        fileSize = file.size();
        exposureTime = file.created();

        if (needs_update) {
            // Update DB.
            mediaTable->updateMedia(id, file.absoluteFilePath(), timestamp, exposureTime,
                                    orientation, fileSize);
        } else {
            // Add to DB.
            id = mediaTable->createIdForMedia(file.absoluteFilePath(), timestamp,
                                              exposureTime, orientation, fileSize);
        }
    } else {
        // Load metadata from DB.
        mediaTable->getRow(id, size, orientation, timestamp, exposureTime);
    }
    video->setSize(QSize(256,256)); // dummy for now
    video->setFileTimestamp(timestamp);
    video->setExposureDateTime(exposureTime);
    video->setId(id);

    return video;
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
 * \param file
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

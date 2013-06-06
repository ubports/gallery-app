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
 * Jim Nelson <jim@yorba.org>
 */

#include "media-source.h"
#include "preview-manager.h"
#include "database/database.h"
#include "database/media-table.h"
#include "event/event.h"
#include "event/event-collection.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include "util/resource.h"
#include "gallery-manager.h"

/*!
 * \brief MediaSource::MediaSource
 */
MediaSource::MediaSource()
    : id_(INVALID_ID)
{
}

/*!
 * \brief MediaSource::MediaSource
 * \param file
 */
MediaSource::MediaSource(const QFileInfo& file)
    : id_(INVALID_ID)
{
    file_ = file;
}

/*!
 * \brief MediaSource::RegisterType
 */
void MediaSource::RegisterType()
{
    qmlRegisterType<MediaSource>("Gallery", 1, 0, "MediaSource");
}

/*!
 * \brief MediaSource::file
 * \return
 */
QFileInfo MediaSource::file() const
{
    return file_;
}

/*!
 * \brief MediaSource::path
 * \return
 */
QUrl MediaSource::path() const
{
    return QUrl::fromLocalFile(file_.absoluteFilePath());
}

/*!
 * \brief MediaSource::gallery_path
 * \return
 */
QUrl MediaSource::gallery_path() const
{
    return GalleryStandardImageProvider::ToURL(file_);
}

/*!
 * \brief MediaSource::preview_file
 * \return
 */
QFileInfo MediaSource::preview_file() const
{
    return GalleryManager::instance()->preview_manager()->PreviewFileFor(file_);
}

/*!
 * \brief MediaSource::preview_path
 * \return
 */
QUrl MediaSource::preview_path() const
{
    return QUrl::fromLocalFile(preview_file().absoluteFilePath());
}

/*!
 * \brief MediaSource::gallery_preview_path
 * \return
 */
QUrl MediaSource::gallery_preview_path() const
{
    return GalleryStandardImageProvider::ToURL(file_);
}

/*!
 * \brief MediaSource::thumbnail_file
 * \return
 */
QFileInfo MediaSource::thumbnail_file() const
{
    return GalleryManager::instance()->preview_manager()->ThumbnailFileFor(file_);
}

/*!
 * \brief MediaSource::thumbnail_path
 * \return
 */
QUrl MediaSource::thumbnail_path() const
{
    return QUrl::fromLocalFile(thumbnail_file().absoluteFilePath());
}

/*!
 * \brief MediaSource::gallery_thumbnail_path
 * \return
 */
QUrl MediaSource::gallery_thumbnail_path() const
{
    return GalleryThumbnailImageProvider::ToURL(file_);
}

/*!
 * \brief MediaSource::Image
 * \param respect_orientation
 * \return
 */
QImage MediaSource::Image(bool respect_orientation, const QSize &scaleSize)
{
    // QML data types cannot be abstract, so return a null image
    return QImage();
}

/*!
 * \brief MediaSource::orientation
 * \return
 */
Orientation MediaSource::orientation() const
{
    // Default is identity orientation; subclasses should parse source's metadata
    // and return proper value
    return TOP_LEFT_ORIGIN;
}

/*!
 * \brief MediaSource::exposure_date_time
 * \return
 */
QDateTime MediaSource::exposure_date_time() const
{
    return QDateTime();
}

/*!
 * \brief MediaSource::size
 * \return
 */
const QSize& MediaSource::size()
{
    if (!is_size_set()) {
        // This is potentially very slow, so you should set the size as early as
        // possible to avoid this.
        QImage image = Image();
        set_size(image.size());
    }

    return size_;
}

/*!
 * \brief MediaSource::set_size
 * \param size
 */
void MediaSource::set_size(const QSize& size)
{
    if (size_ == size)
        return;

    size_ = size;
    notify_size_altered();
}

/*!
 * \brief MediaSource::is_size_set
 * \return
 */
bool MediaSource::is_size_set() const
{
    return size_.isValid();
}

/*!
 * \brief MediaSource::exposure_date
 * \return
 */
QDate MediaSource::exposure_date() const
{
    return exposure_date_time().date();
}

/*!
 * \brief MediaSource::exposure_time_of_day
 * \return
 */
QTime MediaSource::exposure_time_of_day() const
{
    return exposure_date_time().time();
}

/*!
 * \brief MediaSource::exposure_time_t
 * \return
 */
int MediaSource::exposure_time_t() const
{
    return (int) exposure_date_time().toTime_t();
}

/*!
 * \brief MediaSource::FindEvent
 * \return
 */
Event* MediaSource::FindEvent()
{
    return GalleryManager::instance()->event_collection()->EventForMediaSource(this);
}

/*!
 * \brief MediaSource::QmlFindEvent
 * \return
 */
QVariant MediaSource::QmlFindEvent()
{
    return QVariant::fromValue(FindEvent());
}

/*!
 * \brief MediaSource::busy
 * \return
 */
bool MediaSource::busy()
{
    return busy_;
}

/*!
 * \brief MediaSource::set_id
 * \param id
 */
void MediaSource::set_id(qint64 id)
{
    id_ = id;
}

/*!
 * \brief MediaSource::get_id
 * \return
 */
qint64 MediaSource::get_id() const
{
    return id_;
}

/*!
 * \brief MediaSource::set_busy
 * \param busy
 */
void MediaSource::set_busy(bool busy)
{
    if (busy == busy_)
        return;

    busy_ = busy;
    emit busyChanged();
}

/*!
 * \brief MediaSource::maxSize
 * \return
 */
int MediaSource::maxSize() const
{
    return GalleryManager::instance()->resource()->maxTextureSize();
}

/*!
 * \brief MediaSource::DestroySource \reimp
 * \param delete_backing
 * \param as_orphan
 */
void MediaSource::DestroySource(bool delete_backing, bool as_orphan)
{
    if (delete_backing) {
        if (!QFile::remove(file_.absoluteFilePath()))
            qDebug("Unable to delete media file %s", qPrintable(file_.absoluteFilePath()));
    }
}

/*!
 * \brief MediaSource::notify_data_altered
 */
void MediaSource::notify_data_altered()
{
    emit data_altered();
}

/*!
 * \brief MediaSource::notify_size_altered
 */
void MediaSource::notify_size_altered()
{
    emit size_altered();

    if (id_ != INVALID_ID)
        GalleryManager::instance()->database()->get_media_table()->set_media_size(id_, size_);
}

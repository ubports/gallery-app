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

// database
#include "database.h"
#include "media-table.h"

#include <QUrl>

/*!
 * \brief MediaSource::MediaSource
 */
MediaSource::MediaSource()
    : m_id(INVALID_ID),
      m_exposureDateTime(),
      m_busy(false),
      m_mediaTable(0)
{
}

/*!
 * \brief MediaSource::MediaSource
 * \param file
 */
MediaSource::MediaSource(const QFileInfo& file)
    : m_id(INVALID_ID),
      m_exposureDateTime(),
      m_busy(false),
      m_mediaTable(0)
{
    m_file = file;
}

/*!
 * \brief MediaSource::type the type of the media
 * \return
 */
MediaSource::MediaType MediaSource::type() const
{
    return None;
}

/*!
 * \brief MediaSource::file
 * \return
 */
QFileInfo MediaSource::file() const
{
    return m_file;
}

/*!
 * \brief MediaSource::path
 * \return
 */
QUrl MediaSource::path() const
{
    return QUrl::fromLocalFile(m_file.absoluteFilePath());
}

/*!
 * \brief MediaSource::Image
 * \param respectOrientation
 * \return
 */
QImage MediaSource::image(bool respectOrientation, const QSize &scaleSize)
{
    Q_UNUSED(respectOrientation);
    Q_UNUSED(scaleSize);
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
 * \brief MediaSource::exposureDateTime
 * \return
 */
const QDateTime& MediaSource::exposureDateTime() const
{
    return m_exposureDateTime;
}

/*!
 * \brief MediaSource::setExposureDateTime
 * \param exposure_time
 */
void MediaSource::setExposureDateTime(const QDateTime& exposureTime)
{
    if (m_exposureDateTime == exposureTime)
        return;

    m_exposureDateTime = exposureTime;
    emit exposureDateTimeChanged();
}

/*!
 * \brief MediaSource::fileTimestamp
 * \return The timestamp of the media file
 */
const QDateTime &MediaSource::fileTimestamp() const
{
    return m_fileTimestamp;
}

/*!
 * \brief MediaSource::setFileTimestamp
 * \param timestamp
 */
void MediaSource::setFileTimestamp(const QDateTime& timestamp)
{
    m_fileTimestamp = timestamp;
}

/*!
 * \brief MediaSource::size
 * \return
 */
const QSize& MediaSource::size()
{
    if (!isSizeSet()) {
        // This is potentially very slow, so you should set the size as early as
        // possible to avoid this.
        QImage fullImage = image();
        setSize(fullImage.size());
    }

    return m_size;
}

/*!
 * \brief MediaSource::set_size
 * \param size
 */
void MediaSource::setSize(const QSize& size)
{
    if (m_size == size)
        return;

    m_size = size;
    notifySizeChanged();
}

/*!
 * \brief MediaSource::is_size_set
 * \return
 */
bool MediaSource::isSizeSet() const
{
    return m_size.isValid();
}

/*!
 * \brief MediaSource::exposure_date
 * \return
 */
QDate MediaSource::exposureDate() const
{
    return exposureDateTime().date();
}

/*!
 * \brief MediaSource::exposure_time_of_day
 * \return
 */
QTime MediaSource::exposureTimeOfDay() const
{
    return exposureDateTime().time();
}

/*!
 * \brief MediaSource::exposure_time_t
 * \return
 */
int MediaSource::exposureTime_t() const
{
    return (int) exposureDateTime().toTime_t();
}

/*!
 * \brief MediaSource::busy
 * \return
 */
bool MediaSource::busy() const
{
    return m_busy;
}

/*!
 * \brief MediaSource::setMediaTable
 * \param mediaTable
 */
void MediaSource::setMediaTable(MediaTable *mediaTable)
{
    m_mediaTable = mediaTable;
}

/*!
 * \brief MediaSource::set_id
 * \param id
 */
void MediaSource::setId(qint64 id)
{
    m_id = id;
}

/*!
 * \brief MediaSource::get_id
 * \return
 */
qint64 MediaSource::id() const
{
    return m_id;
}

/*!
 * \brief MediaSource::set_busy
 * \param busy
 */
void MediaSource::setBusy(bool busy)
{
    if (busy == m_busy)
        return;

    m_busy = busy;
    emit busyChanged(m_busy);
}

/*!
 * \brief MediaSource::destroySource \reimp
 * \param deleteBacking
 * \param asOrphan
 */
void MediaSource::destroySource(bool deleteBacking, bool asOrphan)
{
    if (deleteBacking) {
        if (!QFile::remove(m_file.absoluteFilePath()))
            qDebug("Unable to delete media file %s", qPrintable(m_file.absoluteFilePath()));
    }
}

/*!
 * \brief MediaSource::notifyDataChanged
 */
void MediaSource::notifyDataChanged()
{
    emit dataChanged();
}

/*!
 * \brief MediaSource::notifySizeChanged
 */
void MediaSource::notifySizeChanged()
{
    emit sizeChanged();

    if (m_id != INVALID_ID && m_mediaTable)
        m_mediaTable->setMediaSize(m_id, m_size);
}

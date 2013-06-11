/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Eric Gregory <eric@yorba.org>
 */

#include "media-table.h"
#include "database.h"

#include <QApplication>
#include <QtSql>

/*!
 * \brief MediaTable::MediaTable
 * \param db
 * \param parent
 */
MediaTable::MediaTable(Database* db, QObject* parent)
    : QObject(parent), m_db(db)
{
}

/*!
 * \brief MediaTable::verifyFiles
 * Runs though the table, removes references to files
 * that have been deleted from disk.
 */
void MediaTable::verifyFiles()
{
    QSqlQuery query(*m_db->getDB());
    QList<qint64> to_delete;
    query.prepare("SELECT id, filename FROM MediaTable");
    if (!query.exec())
        m_db->logSqlError(query);

    // Stat each file. Make a list of files that no longer exist.
    while (query.next()) {
        // stat'ing and sync'ing file info over even several hundred photos is an
        // expensive operation since it involves lots of I/O, so spin the event
        // loop so that the UI remains responsive
        QApplication::processEvents();

        qint64 id = query.value(0).toLongLong();
        QFile file(query.value(1).toString());

        if (!file.exists())
            to_delete.append(id);
    }

    // Delete any references to non-existent files.
    m_db->getDB()->transaction();
    foreach (qint64 id, to_delete) {
        // spin the event loop so that the UI remains responsive
        QApplication::processEvents();

        remove(id);
    }
    
    m_db->getDB()->commit();
}

/*!
 * \brief MediaTable::getIdForMedia Returns the row ID for the given photo.
 * \param filename
 * \return Returns the row ID for the given photo. If none exists, -1 will be returned.
 */
qint64 MediaTable::getIdForMedia(const QString& filename)
{
    // If there's a row for this file, return the ID.
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT id FROM MediaTable WHERE filename = :filename");
    query.bindValue(":filename", filename);
    if (!query.exec())
        m_db->logSqlError(query);

    if (query.next())
        return query.value(0).toLongLong();

    // No row found.
    return -1;
}

/*!
 * \brief MediaTable::createIdForMedia Creates a row for the given photo and returns the new ID
 * \param filename
 * \param timestamp
 * \param exposureTime
 * \param originalOrientation
 * \param filesize
 * \return
 */
qint64 MediaTable::createIdForMedia(const QString& filename,
                                       const QDateTime& timestamp, const QDateTime& exposureTime,
                                       Orientation originalOrientation, qint64 filesize)
{
    // Add the row.
    QSqlQuery query(*m_db->getDB());
    query.prepare("INSERT INTO MediaTable (filename, timestamp, exposure_time, "
                  "original_orientation, filesize) VALUES (:filename, :timestamp, "
                  ":exposure_time, :original_orientation, :filesize)");
    query.bindValue(":filename", filename);
    query.bindValue(":timestamp", timestamp.toMSecsSinceEpoch());
    query.bindValue(":exposure_time", exposureTime.toMSecsSinceEpoch());
    query.bindValue(":original_orientation", originalOrientation);
    query.bindValue(":filesize", filesize);
    if (!query.exec())
        m_db->logSqlError(query);

    return query.lastInsertId().toLongLong();
}

/*!
 * \brief MediaTable::updateMedia Updates a given row
 * \param mediaId
 * \param filename
 * \param timestamp
 * \param exposureTime
 * \param originalOrientation
 * \param filesize
 */
void MediaTable::updateMedia(qint64 mediaId, const QString& filename,
                              const QDateTime& timestamp, const QDateTime& exposureTime,
                              Orientation originalOrientation, qint64 filesize)
{
    // Add the row.
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE MediaTable SET filename = :filename, "
                  "timestamp = :timestamp, exposure_time = :exposure_time, "
                  "original_orientation = :original_orientation, "
                  "filesize = :filesize WHERE id = :id");
    query.bindValue(":filename", filename);
    query.bindValue(":timestamp", timestamp.toMSecsSinceEpoch());
    query.bindValue(":exposure_time", exposureTime.toMSecsSinceEpoch());
    query.bindValue(":original_orientation", originalOrientation);
    query.bindValue(":filesize", filesize);
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief MediaTable::remove Removes a photo from the database.
 * \param mediaId
 */
void MediaTable::remove(qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("DELETE FROM MediaTable WHERE id = :id");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief MediaTable::getMediaSize
 * \param mediaId
 * \return
 */
QSize MediaTable::getMediaSize(qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT width, height FROM MediaTable WHERE id = :id LIMIT 1");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);

    QSize size;
    if (query.next()) {
        int width = query.value(0).toInt();
        int height = query.value(1).toInt();
        if (width > 0 && height > 0)
            size = QSize(width, height);
    }

    return size;
}

/*!
 * \brief MediaTable::setMediaSize
 * \param mediaId
 * \param size
 */
void MediaTable::setMediaSize(qint64 mediaId, const QSize& size)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE MediaTable SET width = :width, height = :height "
                  "WHERE id = :id");
    query.bindValue(":id", mediaId);
    query.bindValue(":width", size.width());
    query.bindValue(":height", size.height());
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief MediaTable::setOriginalOrientation
 * \param mediaId
 * \param orientation
 */
void MediaTable::setOriginalOrientation(qint64 mediaId, const Orientation& orientation)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("UPDATE MediaTable SET orientation = :orientation WHERE id = :id");
    query.bindValue(":id", mediaId);
    query.bindValue(":orientation", orientation);
    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief MediaTable::getFileTimestamp
 * \param mediaId
 * \return
 */
QDateTime MediaTable::getFileTimestamp(qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT timestamp FROM MediaTable WHERE id = :id");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);

    QDateTime timestamp;
    if (query.next()) {
        timestamp.setMSecsSinceEpoch(query.value(0).toLongLong());
    }

    return timestamp;
}

/*!
 * \brief MediaTable::getExposureTime
 * \param mediaId
 * \return
 */
QDateTime MediaTable::getExposureTime(qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT exposure_time FROM MediaTable WHERE id = :id");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);

    QDateTime exposure_time;
    if (query.next()) {
        exposure_time.setMSecsSinceEpoch(query.value(0).toLongLong());
    }

    return exposure_time;
}

/*!
 * \brief MediaTable::getRow Gets a row that already exists
 * \param mediaId
 * \param size
 * \param originalOrientation
 * \param fileTimestamp
 * \param exposureDateTime
 */
void MediaTable::getRow(qint64 mediaId, QSize& size, Orientation& 
                         originalOrientation, QDateTime& fileTimestamp, QDateTime& exposureDateTime)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT width, height, timestamp, exposure_time, "
                  "original_orientation FROM MediaTable WHERE id = :id LIMIT 1");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);

    if (!query.next())
        m_db->logSqlError(query);

    size = QSize(query.value(0).toInt(), query.value(1).toInt());

    fileTimestamp.setMSecsSinceEpoch(query.value(2).toLongLong());
    exposureDateTime.setMSecsSinceEpoch(query.value(3).toLongLong());
    originalOrientation = static_cast<Orientation>(query.value(4).toInt());
}

/*!
 * \brief MediaTable::rowNeedsUpdate
 * Returns true if row is from an older schema.  In that case, update_media()
 * should be called to repopulate the row.
 * \param mediaId
 * \return
 */
bool MediaTable::rowNeedsUpdate(qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT 1 FROM MediaTable WHERE timestamp IS NULL "
                  "AND id = :id LIMIT 1");
    query.bindValue(":id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);

    return query.next();
}

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

#include <QDateTime>
#include <QDebug>
#include <QList>

class MediaDataRow
{
public:
    qint64 id;
    QString filename;
    QDateTime timestamp;
    QDateTime exposureTime;
    Orientation originalOrientation;
    qint64 filesize;
    int width;
    int height;
};

static qint64 mediaLastId = 0;
static QList<MediaDataRow> mediaFakeTable;

// for controlling the fake MediaTable from the tests
void setOrientationOfFirstRow(Orientation orientation)
{
    mediaFakeTable[0].originalOrientation = orientation;
}

MediaTable::MediaTable(Database* db, QObject* parent)
    : QObject(parent), m_db(db)
{
    mediaLastId = 0;
    mediaFakeTable.clear();
}

qint64 MediaTable::getIdForMedia(const QString& filename)
{
    foreach (const MediaDataRow &row, mediaFakeTable) {
        if (row.filename == filename)
            return row.id;
    }
    return -1;
}

qint64 MediaTable::createIdForMedia(const QString& filename,
                                       const QDateTime& timestamp, const QDateTime& exposureTime,
                                       Orientation originalOrientation, qint64 filesize, QSize size)
{
    MediaDataRow row;
    row.id = mediaLastId;
    mediaLastId++;
    row.filename = filename;
    row.timestamp = timestamp;
    row.exposureTime = exposureTime;
    row.originalOrientation = originalOrientation;
    row.filesize = filesize;
    row.height = size.height();
    row.width = size.width();
    mediaFakeTable.append(row);
    return row.id;
}

void MediaTable::updateMedia(qint64 mediaId, const QString& filename,
                              const QDateTime& timestamp, const QDateTime& exposureTime,
                              Orientation originalOrientation, qint64 filesize)
{
    foreach (MediaDataRow row, mediaFakeTable) {
        if (row.id == mediaId) {
            row.filename = filename;
            row.timestamp = timestamp;
            row.exposureTime = exposureTime;
            row.originalOrientation = originalOrientation;
            row.filesize = filesize;
            return;
        }
    }
}

void MediaTable::remove(qint64 mediaId)
{
}

QSize MediaTable::getMediaSize(qint64 mediaId)
{
    return QSize();
}

void MediaTable::setMediaSize(qint64 mediaId, const QSize& size)
{
}

void MediaTable::emitAllRows()
{
}

void MediaTable::getRow(qint64 mediaId, QSize& size, Orientation& 
                         originalOrientation, QDateTime& fileTimestamp, QDateTime& exposureDateTime)
{
    foreach (MediaDataRow row, mediaFakeTable) {
        if (row.id == mediaId) {
            fileTimestamp = row.timestamp;
            exposureDateTime = row.exposureTime;
            originalOrientation = row.originalOrientation;
            return;
        }
    }
}

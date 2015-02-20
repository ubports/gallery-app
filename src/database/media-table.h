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

#ifndef MEDIATABLE_H
#define MEDIATABLE_H

// util
#include "orientation.h"

#include <QObject>

class Database;

/*!
 * \brief The MediaTable class
 */
class MediaTable : public QObject
{
    Q_OBJECT

public:
    explicit MediaTable(Database* db, QObject *parent = 0);

    qint64 getIdForMedia(const QString& filename);

    qint64 createIdForMedia(const QString& filename, const QDateTime& timestamp,
                            const QDateTime& exposureTime, Orientation originalOrientation,
                            qint64 filesize, QSize size);

    void updateMedia(qint64 mediaId, const QString& filename,
                      const QDateTime& timestamp, const QDateTime& exposureTime,
                      Orientation originalOrientation, qint64 filesize);

    void getRow(qint64 mediaId, QSize& size, Orientation& originalOrientation,
                 QDateTime& fileTimestamp, QDateTime& exposureDateTime);

    void remove(qint64 mediaId);

    QSize getMediaSize(qint64 mediaId);
    void setMediaSize(qint64 mediaId, const QSize& size);

    void setOriginalOrientation(qint64 mediaId, const Orientation& orientation);

    QDateTime getFileTimestamp(qint64 mediaId);

    QDateTime getExposureTime(qint64 mediaId);

    void emitAllRows();

signals:
    void row(qint64 mediaId, const QString& filename, const QSize& size,
             const QDateTime& timestamp, const QDateTime& exposureTime,
             Orientation originalOrientation, qint64 filesize);

private:
    Database* m_db;
};

#endif // MEDIATABLE_H

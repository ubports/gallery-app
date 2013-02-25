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

#include <QObject>

#include "photo/photo-metadata.h"

class Database;

/*!
 * \brief The MediaTable class
 */
class MediaTable : public QObject
{
    Q_OBJECT

public:
    explicit MediaTable(Database* db, QObject *parent = 0);

    void verify_files();

    qint64 get_id_for_media(const QString& filename);

    qint64 create_id_for_media(const QString& filename, const QDateTime& timestamp,
                               const QDateTime& exposure_time, Orientation original_orientation,
                               qint64 filesize);

    void update_media(qint64 media_id, const QString& filename,
                      const QDateTime& timestamp, const QDateTime& exposure_time,
                      Orientation original_orientation, qint64 filesize);

    void get_row(qint64 media_id, QSize& size, Orientation& original_orientation,
                 QDateTime& file_timestamp, QDateTime& exposure_date_time);

    void remove(qint64 mediaId);

    QSize get_media_size(qint64 media_id);
    void set_media_size(qint64 media_id, const QSize& size);

    void set_original_orientation(qint64 media_id, const Orientation& orientation);

    QDateTime get_file_timestamp(qint64 media_id);

    QDateTime get_exposure_time(qint64 media_id);

    bool row_needs_update(qint64 media_id);

private:
    Database* db_;
};

#endif // MEDIATABLE_H

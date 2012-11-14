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

#include "database.h"

class Database;

class MediaTable : public QObject {
  Q_OBJECT
  
 public:
  explicit MediaTable(Database* db, QObject *parent = 0);
  
  // Runs though the table, removes references to files
  // that have been deleted from disk.
  void verify_files();
  
  // Returns the row ID for the given photo.  If none exists,
  // -1 will be returned.
  qint64 get_id_for_media(QString filename);
  
  // Creates a row for the given photo and returns the new ID.
  qint64 create_id_for_media(QString filename, qint64 timestamp,
    qint64 exposure_time, Orientation original_orientation);
  
  // Updates an existing row.
  void update_media(qint64 media_id, QString filename, qint64 timestamp,
    Orientation original_orientation);
  
  // Gets a row that already exists.
  void get_row(qint64 media_id, QSize& size, Orientation& original_orientation,
    QDateTime& file_timestamp, QDateTime& exposure_date_time);
  
  // Removes a photo from the database.
  void remove(qint64 mediaId);

  QSize get_media_size(qint64 media_id);
  void set_media_size(qint64 media_id, const QSize& size);
  
  void set_original_orientation(qint64 media_id, const Orientation& orientation);
  
  QDateTime get_file_timestamp(qint64 media_id);
  
  QDateTime get_exposure_time(qint64 media_id);
  
 private:
  Database* db_;
};

#endif // MEDIATABLE_H

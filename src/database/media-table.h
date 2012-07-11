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
  
  // Returns the row ID for a given photo.  If none exists,
  // one will be created.
  qint64 get_id_for_media(QString filename);
  
  // Removes a photo from the database.
  void remove(qint64 mediaId);

  QSize get_media_size(qint64 media_id);
  void set_media_size(qint64 media_id, const QSize& size);
  
 private:
  Database* db_;
};

#endif // MEDIATABLE_H

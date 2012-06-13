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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

#include <QtSql>
#include <QSqlTableModel>
#include <QString>
#include <QFile>

#include "album-table.h"
#include "media-table.h"

class AlbumTable;
class MediaTable;

const qint64 INVALID_ID = -1;

class Database : public QObject {
  Q_OBJECT
  
 public:
  
  static void Init(const QDir& db_dir);
  static Database* instance();
  
  ~Database();
  
  // Logs a SQL error.
  void log_sql_error(QSqlQuery& q) const;
  QSqlDatabase* get_db();
  
  AlbumTable* get_album_table() const;
  
  MediaTable* get_media_table() const;
  
 private:
  Database(const QDir& db_dir, QObject* parent = 0);
  
  // Get/set schema version.
  int get_schema_version() const;
  void set_schema_version(int version);
  
  // Upgrades the schema from current_version to the latest & greatest.
  void upgrade_schema(int current_version);
  
  // Executes a text file containing SQL commands.
  bool execute_sql_file(QFile& file);
  
  // Returns the directory where the .sql files live.
  QDir get_sql_dir();
  
  static Database* instance_;
  QSqlDatabase db_;
  
  AlbumTable* album_table_;
  MediaTable* media_table_;
};

#endif // DATABASE_H

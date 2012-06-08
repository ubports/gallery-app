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

#include "database.h"

Database* Database::instance_ = NULL;


void Database::Init(const QDir& db_dir, const QDir& sql_dir) {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new Database(db_dir, sql_dir, NULL);
}

Database* Database::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

Database::Database(const QDir& db_dir, const QDir& sql_dir,
                 QObject* parent) : QObject(parent) {
  this->sql_directory_ = sql_dir;
  
  // Setup database.
  db_ = QSqlDatabase::addDatabase("QSQLITE");
  
  album_table_ = new AlbumTable(this, this);
  media_table_ = new MediaTable(this, this);
  
  // Open the database.
  db_.setDatabaseName(db_dir.path() + "/gallery.sqlite");
  if (!db_.open()) {
    qDebug("Could not open db.");
    return;
  }
  
  // Turn synchronous off.
  QSqlQuery query(db_);
  if (!query.exec("PRAGMA synchronous = OFF")) {
    log_sql_error(query);
    return;
  }
  
  // Enable foreign keys.
  if (!query.exec("PRAGMA foreign_keys = ON")) {
    log_sql_error(query);
    return;
  }
  
  // Update if needed.
  upgrade_schema(get_schema_version());
}

Database::~Database() {
  delete album_table_;
  delete media_table_;
}

void Database::log_sql_error(QSqlQuery& q) const {
  qDebug() << "SQLite error: " << q.lastError();
  qDebug() << "SQLite string: " << q.lastQuery();
}

int Database::get_schema_version() const {
  QSqlQuery query(db_);
  if (!query.exec("PRAGMA user_version") || !query.next()) {
    log_sql_error(query);
    return -1;
  }
  
  return query.value(0).toInt();
}

void Database::set_schema_version(int version) {
  // Must use string concats here since prepared statements
  // appear not to work with PRAGMAs.
  QSqlQuery query(db_);
  if (!query.exec("PRAGMA user_version = " + QString::number(version)))
    log_sql_error(query);
}

void Database::upgrade_schema(int current_version) {
  int version = current_version + 1;
  for (;; version++) {
    // Check for the existence of an updated db file.
    // Filename format is n.sql, where n is the schema version number.
    QFile file(sql_directory_.path() + "/" + QString::number(version) + ".sql");
    if (!file.exists())
      return;
    
    if (!execute_sql_file(file))
      return;
    
    // Update version.
    set_schema_version(version);
  }
}

bool Database::execute_sql_file(QFile& file) {
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Could not open file: " << file.fileName();
    return false;
  }
  
  // Read entire file into a string.
  QString sql;
  QTextStream in(&file);
  while (!in.atEnd())
    sql += in.readLine() + "\n";
  file.close();
  
  // Split string at semi-colons to break into multiple statements.
  // This is due to the SQLite driver's inability to handle multiple
  // statements in a single string.
  QStringList statements = sql.split(";", QString::SkipEmptyParts);
  foreach (QString statement, statements) {
    if (statement.trimmed() == "")
      continue;
    
    // Execute each statement.
    QSqlQuery query(db_);
    if (!query.exec(statement)) {
      qDebug() << "Error executing database file: " << file.fileName();
      log_sql_error(query);
    }
  }
  
  return true;
}

AlbumTable* Database::get_album_table() const {
  return album_table_;
}

MediaTable* Database::get_media_table() const {
  return media_table_;
}

QSqlDatabase* Database::get_db() {
  return &db_;
}

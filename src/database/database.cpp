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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "database.h"

#include "util/resource.h"

const QString Database::DATABASE_DIR = ".database";

Database* Database::instance_ = NULL;


void Database::Init(const QDir& pictures_dir, QObject* parent) {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new Database(pictures_dir, parent);
}

Database* Database::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

Database::Database(const QDir& pictures_dir, QObject* parent) :
    QObject(parent) {
  
  QDir db_dir(pictures_dir);
  db_dir.mkdir(DATABASE_DIR);
  db_dir.cd(DATABASE_DIR);
  db_dir_ = db_dir;

  album_table_ = new AlbumTable(this, this);
  media_table_ = new MediaTable(this, this);
  photo_edit_table_ = new PhotoEditTable(this, this);
  
  // Open the database.
  if (!open_db())
    restore_from_backup();
  
  // Attempt a query to make sure the DB is valid.
  QSqlQuery test_query(db_);
  if (!test_query.exec("SELECT * FROM SQLITE_MASTER LIMIT 1")) {
    log_sql_error(test_query);
    restore_from_backup();
  }
  
  QSqlQuery query(db_);
  // Turn synchronous off.
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
  delete photo_edit_table_;
  
  create_backup();
}

void Database::log_sql_error(QSqlQuery& q) const {
  qDebug() << "SQLite error: " << q.lastError();
  qDebug() << "SQLite string: " << q.lastQuery();
}

bool Database::open_db() {
  db_ = QSqlDatabase::addDatabase("QSQLITE");
  db_.setDatabaseName(get_db_name());
  if (!db_.open()) {
    qDebug() << "Error opening DB: " << db_.lastError().text();
    return false;
  }
  
  return true;
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
    QFile file(get_sql_dir().path() + "/" + QString::number(version) + ".sql");
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

PhotoEditTable* Database::get_photo_edit_table() const {
  return photo_edit_table_;
}

QSqlDatabase* Database::get_db() {
  return &db_;
}

QDir Database::get_sql_dir() {
  return QDir(Resource::instance()->get_rc_url("sql").path());
}

void Database::restore_from_backup() {
  db_.close();
  
  // Remove existing DB.
  QFile bad_db(get_db_name());
  if (!bad_db.remove())
    qDebug() << "Could not remove old file.";
  
  // Copy the backup, if it exists.
  QFile file(get_db_backup_name());
  if (file.exists()) {
    qDebug() << "Restoring database from backup.";
    file.copy(get_db_name());
  }
  
  open_db();
}

void Database::create_backup() {
  QFile old_backup(get_db_backup_name());
  if (!old_backup.remove())
    qDebug() << "Could not remove existing backup.";
  
  // Copy the database to the backup.
  QFile file(get_db_name());
  file.copy(get_db_backup_name());
}

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

MediaTable::MediaTable(Database* db, QObject* parent) : QObject(parent), db_(db)
{
}

void MediaTable::verify_files() {
  QSqlQuery query(*db_->get_db());
  QList<qint64> to_delete;
  query.prepare("SELECT id, filename FROM MediaTable");
  if (!query.exec())
    db_->log_sql_error(query);
  
  // Stat each file. Make a list of files that no longer exist.
  while (query.next()) {
    qint64 id = query.value(0).toLongLong();
    QFile file(query.value(1).toString());
    
    if (!file.exists())
      to_delete.append(id);
  }
  
  // Delete any references to non-existent files.
  db_->get_db()->transaction();
  foreach (qint64 id, to_delete)
    remove(id);
    
  db_->get_db()->commit();
}

qint64 MediaTable::get_id_for_media(QString filename) {
  // If there's a row for this file, return the ID.
  QSqlQuery query(*db_->get_db());
  query.prepare("SELECT id FROM MediaTable WHERE filename = :filename");
  query.bindValue(":filename", filename);
  if (!query.exec())
    db_->log_sql_error(query);
  
  if (query.next())
    return query.value(0).toLongLong();
  
  // Need to add the photo to the DB.
  query.prepare("INSERT INTO MediaTable (filename) VALUES (:filename)");
  query.bindValue(":filename", filename);
  if (!query.exec())
    db_->log_sql_error(query);
  
  return query.lastInsertId().toLongLong();
}

void MediaTable::remove(qint64 mediaId) {
  QSqlQuery query(*db_->get_db());
  query.prepare("DELETE FROM MediaTable WHERE id = :id");
  query.bindValue(":id", mediaId);
  if (!query.exec())
    db_->log_sql_error(query);
}

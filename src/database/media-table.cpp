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

#include <QApplication>

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
  db_->get_db()->transaction();
  foreach (qint64 id, to_delete) {
    // spin the event loop so that the UI remains responsive
    QApplication::processEvents();

    remove(id);
  }
    
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

QSize MediaTable::get_media_size(qint64 media_id) {
  QSqlQuery query(*db_->get_db());
  query.prepare("SELECT width, height FROM MediaTable WHERE id = :id");
  query.bindValue(":id", media_id);
  if (!query.exec())
    db_->log_sql_error(query);

  QSize size;
  if (query.next()) {
    int width = query.value(0).toInt();
    int height = query.value(1).toInt();
    if (width > 0 && height > 0)
      size = QSize(width, height);
  }

  return size;
}

void MediaTable::set_media_size(qint64 media_id, const QSize& size) {
  QSqlQuery query(*db_->get_db());
  query.prepare("UPDATE MediaTable SET width = :width, height = :height "
                "WHERE id = :id");
  query.bindValue(":id", media_id);
  query.bindValue(":width", size.width());
  query.bindValue(":height", size.height());
  if (!query.exec())
    db_->log_sql_error(query);
}

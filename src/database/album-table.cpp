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

#include "album-table.h"
#include "album/album-default-template.h"

AlbumTable::AlbumTable(Database* db, QObject* parent) : QObject(parent), db_(db)
{
}

void AlbumTable::get_albums(QList<Album*>* album_list) const {
  QSqlQuery query(*db_->get_db());
  query.prepare("SELECT id, title, subtitle, time_added FROM AlbumTable "
                "ORDER BY time_added DESC");
  if (!query.exec())
    db_->log_sql_error(query);
  
  while (query.next()) {
    qint64 id = query.value(0).toLongLong();
    QString title = query.value(1).toString();
    QString subtitle = query.value(2).toString();
    QDateTime timestamp;
    timestamp.setMSecsSinceEpoch(query.value(3).toLongLong());
    
    Album* a = new Album(*AlbumDefaultTemplate::instance(), title, subtitle);
    a->set_id(id);
    a->set_creation_date_time(timestamp);
    
    album_list->append(a);
  }
}

void AlbumTable::add_album(Album* album) {
  if (album->get_id() != INVALID_ID)
    return; // Nothing to do here.
  
  QSqlQuery query(*db_->get_db());
  query.prepare("INSERT INTO AlbumTable (title, subtitle, time_added) "
                "VALUES (:title, :subtitle, :time_added)");
  query.bindValue(":title", album->title());
  query.bindValue(":subtitle", album->subtitle());
  query.bindValue(":time_added", album->creation_date_time().toMSecsSinceEpoch());
  if (!query.exec())
      db_->log_sql_error(query);
  
  album->set_id(query.lastInsertId().toLongLong());
}

void AlbumTable::remove_album(Album* album) {
  if (album->get_id() == INVALID_ID)
    return; // Nothing to remove.
  
  QSqlQuery query(*db_->get_db());
  query.prepare("DELETE FROM AlbumTable WHERE id = :id");
  query.bindValue(":id", album->get_id());
  if (!query.exec())
      db_->log_sql_error(query);
  
  album->set_id(INVALID_ID);
}

void AlbumTable::attach_to_album(qint64 album_id, qint64 media_id) {
  QSqlQuery query(*db_->get_db());
  query.prepare("INSERT INTO MediaAlbumTable (album_id, media_id) "
                "VALUES (:album_id, :media_id)");
  query.bindValue(":album_id", album_id);
  query.bindValue(":media_id", media_id);
  if (!query.exec())
      db_->log_sql_error(query);
}

void AlbumTable::detach_from_album(qint64 album_id, qint64 media_id) {
  QSqlQuery query(*db_->get_db());
  query.prepare("DELETE FROM MediaAlbumTable WHERE album_id = :album_id AND "
                "media_id = :media_id");
  query.bindValue(":album_id", album_id);
  query.bindValue(":media_id", media_id);
  if (!query.exec())
    db_->log_sql_error(query);
}

void AlbumTable::media_for_album(qint64 album_id, QList<qint64>* list) const {
  QSqlQuery query(*db_->get_db());
  query.prepare("SELECT media_id FROM MediaAlbumTable WHERE "
                "album_id = :album_id");
  query.bindValue(":album_id", album_id);
  if (!query.exec())
    db_->log_sql_error(query);
  
  while (query.next())
    list->append(query.value(0).toLongLong());
}

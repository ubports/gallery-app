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

#ifndef ALBUMTABLE_H
#define ALBUMTABLE_H

#include <QObject>
#include <QList>

#include "album/album.h"
#include "database.h"

class Database;

class AlbumTable : public QObject {
  Q_OBJECT
  
 public:
  explicit AlbumTable(Database* db, QObject* parent = 0);
  
  // Returns a set of all albums.
  void get_albums(QList<Album*>* album_set) const;
  
  // Adds an album to the DB.
  void add_album(Album* album);
  
  // Removes an album from the DB.
  void remove_album(Album* album);
  
  // Updates an existing album in the DB.
  // Note: ID must be set.
  void update_album(Album* album);
  
  // Adds a photo to an album.
  void attach_to_album(qint64 album_id, qint64 media_id);
  
  // Removes a photo from an album.
  void detach_from_album(qint64 album_id, qint64 media_id);
  
  // Returns a list of photos for an album.
  void media_for_album(qint64 album_id, QList<qint64>* list) const;
  
 private:
  Database* db_;
};

#endif // ALBUMTABLE_H

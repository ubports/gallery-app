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

class Album;
class Database;

class AlbumTable : public QObject {
  Q_OBJECT
  
 public:
  explicit AlbumTable(Database* db, QObject* parent = 0);
  
  // Returns a set of all albums.
  void get_albums(QList<Album*>* album_set);
  
  // Adds an album to the DB.
  void add_album(Album* album);
  
  // Removes an album from the DB.
  void remove_album(Album* album);
  
  // Adds a photo to an album.
  void attach_to_album(qint64 album_id, qint64 media_id);
  
  // Removes a photo from an album.
  void detach_from_album(qint64 album_id, qint64 media_id);
  
  // Returns a list of photos for an album.
  void media_for_album(qint64 album_id, QList<qint64>* list) const;
  
  // Sets whether or not an album is open.
  void set_is_closed(qint64 album_id, bool is_closed);
  
  // Sets the current page of the album.
  void set_current_page(qint64 album_id, int page);
  
  // Sets the cover style for the album.
  void set_cover_nickname(qint64 album_id, QString cover_nickname);
  
  // Sets the title of the album.
  void set_title(qint64 album_id, QString title);
  
  // Sets the subtitle of the album.
  void set_subtitle(qint64 album_id, QString subtitle);
  
 private:
  Database* db_;
};

#endif // ALBUMTABLE_H

/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Jim Nelson <jim@yorba.org>
 */

#ifndef GALLERY_ALBUM_COLLECTION_H_
#define GALLERY_ALBUM_COLLECTION_H_

#include <QObject>

#include "album/album.h"
#include "core/container-source-collection.h"
#include "core/data-object.h"

class AlbumCollection : public ContainerSourceCollection {
  Q_OBJECT
  
 signals:
  void album_current_page_contents_altered(Album* album);
  
 public:
  friend class Album;
  
  static void Init();
  
  static AlbumCollection* instance();
  
  static bool CreationDateTimeAscendingComparator(DataObject* a, DataObject* b);
  static bool CreationDateTimeDescendingComparator(DataObject* a, DataObject* b);

 protected:
  virtual void notify_album_current_page_contents_altered(Album* album);
  
 private:
  static AlbumCollection* instance_;
  
  AlbumCollection();
};

#endif  // GALLERY_ALBUM_COLLECTION_H_

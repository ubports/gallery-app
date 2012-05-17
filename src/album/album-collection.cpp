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

#include "album/album-collection.h"

#include "media/media-collection.h"

AlbumCollection *AlbumCollection::instance_ = NULL;

AlbumCollection::AlbumCollection()
  : ContainerSourceCollection("AlbumCollection", CreationDateTimeDescendingComparator) {

  // We need to monitor the media collection so that when photos get removed
  // from the system, they also get removed from all albums.
  QObject::connect(
    MediaCollection::instance(),
    SIGNAL(contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_media_added_removed(const QSet<DataObject*>*,const QSet<DataObject*>*)));
}

void AlbumCollection::Init() {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new AlbumCollection();
}

AlbumCollection* AlbumCollection::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

bool AlbumCollection::CreationDateTimeAscendingComparator(DataObject* a, DataObject* b) {
  return qobject_cast<Album*>(a)->creation_date_time() < qobject_cast<Album*>(b)->creation_date_time();
}

bool AlbumCollection::CreationDateTimeDescendingComparator(DataObject* a, DataObject* b) {
  return CreationDateTimeAscendingComparator(b, a);
}

void AlbumCollection::notify_album_current_page_contents_altered(Album* album) {
  emit album_current_page_contents_altered(album);
}

void AlbumCollection::on_media_added_removed(const QSet<DataObject *> *added,
  const QSet<DataObject *> *removed) {
  if (removed != NULL) {
    // TODO: this could maybe be optimized.  Many albums might not care about
    // the particular photo being added.
    DataObject* album_object;
    foreach (album_object, GetAsSet()) {
      Album* album = qobject_cast<Album*>(album_object);
      Q_ASSERT(album != NULL);

      DataObject* object;
      foreach (object, *removed) {
        MediaSource* media = qobject_cast<MediaSource*>(object);
        Q_ASSERT(media != NULL);

        album->Detach(media);
      }
    }
  }
}

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

#include "media/media-collection.h"

#include <QStringList>
#include <QString>

#include "photo/photo.h"
#include "photo/photo-edit-state.h"
#include "util/collections.h"
#include "database/database.h"
#include "database/media-table.h"

MediaCollection* MediaCollection::instance_ = NULL;

MediaCollection::MediaCollection(const QDir& directory)
  : SourceCollection("MediaCollection"), directory_(directory) {
  directory_.setFilter(QDir::Files);
  directory_.setSorting(QDir::Name);
  
  // By default, sort all media by its exposure date time, descending
  SetComparator(ExposureDateTimeDescendingComparator);
  
  QSet<DataObject*> photos;
  QStringList filenames = directory_.entryList();
  QString filename;
  foreach (filename, filenames) {
    // stat'ing and sync'ing file info over even several hundred photos is an
    // expensive operation since it involves lots of I/O, so spin the event
    // loop so that the UI remains responsive
    QApplication::processEvents();
    
    QFileInfo file(directory_, filename);
    
    Photo *p = Photo::Load(file);
    if (!p)
      continue;
    
    photos.insert(p);
    id_map_.insert(p->get_id(), p);
  }
  
  AddMany(photos);
}

void MediaCollection::Init(const QDir& directory) {
  Q_ASSERT(instance_ == NULL);
  
  instance_ = new MediaCollection(directory);
}

MediaCollection* MediaCollection::instance() {
  Q_ASSERT(instance_ != NULL);
  
  return instance_;
}

const QDir& MediaCollection::directory() const {
  return directory_;
}

bool MediaCollection::ExposureDateTimeAscendingComparator(DataObject* a,
  DataObject* b) {
  return
    qobject_cast<MediaSource*>(a)->exposure_date_time() < qobject_cast<MediaSource*>(b)->exposure_date_time();
}

bool MediaCollection::ExposureDateTimeDescendingComparator(DataObject* a,
  DataObject* b) {
  return
    qobject_cast<MediaSource*>(a)->exposure_date_time() > qobject_cast<MediaSource*>(b)->exposure_date_time();
}

MediaSource* MediaCollection::mediaForId(qint64 id) {
  return id_map_.contains(id) ? qobject_cast<MediaSource*>(id_map_[id]) : NULL;
}

void MediaCollection::notify_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  
  SourceCollection::notify_contents_altered(added, removed);
  
  // Track IDs of objects as they're added and removed.
  if (added != NULL) { 
    QSetIterator<DataObject*> i(*added);
    while (i.hasNext()) {
      DataObject* o = i.next();
      id_map_.insert(qobject_cast<MediaSource*>(o)->get_id(), o);

      Photo* p = qobject_cast<Photo*>(o);
      if (p != NULL) {
        file_photo_map_.insert(p->file().absoluteFilePath(), p);
      }
    }
  }
  
  if (removed != NULL) {
    QSetIterator<DataObject*> i(*removed);
    while (i.hasNext()) {
      DataObject* o = i.next();
      MediaSource* media = qobject_cast<MediaSource*>(o);

      Photo* p = qobject_cast<Photo*>(o);
      if (p != NULL) {
        file_photo_map_.remove(p->file().absoluteFilePath());
      }

      id_map_.remove(media->get_id());
      
      // TODO: In the future we may want to do this in the Destroy method
      // (as defined in DataSource) if we want to differentiate between
      // removing the photo and "deleting the backing file."
      Database::instance()->get_media_table()->remove(media->get_id());
    }
  }
}


Photo* MediaCollection::photoFromFileinfo(QFileInfo file_to_load) {
  return file_photo_map_.value(file_to_load.absoluteFilePath(),NULL);
}

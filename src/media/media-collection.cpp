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

#include <QApplication>
#include <QFileInfo>
#include <QStringList>
#include <QString>

#include "media-collection.h"
#include "core/gallery-manager.h"
#include "photo/photo.h"
#include "database/database.h"
#include "database/media-table.h"

/*!
 * \brief MediaCollection::MediaCollection
 * \param directory
 */
MediaCollection::MediaCollection(const QDir& directory)
  : SourceCollection("MediaCollection"), directory_(directory)
{
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

/*!
 * \brief MediaCollection::directory
 * \return
 */
const QDir& MediaCollection::directory() const
{
  return directory_;
}

/*!
 * \brief MediaCollection::ExposureDateTimeAscendingComparator
 * NOTE: this comparator function expects the API contract of
 *       DataObject::number() to return the same value for the same logical
 *       data object across invocations of Gallery. Right now, this contract
 *       is tenuously maintained. See the TODO item in DataObject.h.
 * \param a
 * \param b
 * \return
 */
bool MediaCollection::ExposureDateTimeAscendingComparator(DataObject* a,
  DataObject* b)
{
  QDateTime exptime_a = qobject_cast<MediaSource*>(a)->exposure_date_time();
  QDateTime exptime_b = qobject_cast<MediaSource*>(b)->exposure_date_time();

  return (exptime_a == exptime_b) ?
    (DataCollection::DefaultDataObjectComparator(a, b)) :
    (exptime_a < exptime_b);
}

/*!
 * \brief MediaCollection::ExposureDateTimeDescendingComparator
 * \param a
 * \param b
 * \return
 */
bool MediaCollection::ExposureDateTimeDescendingComparator(DataObject* a,
  DataObject* b)
{
  return !ExposureDateTimeAscendingComparator(a, b);
}

/*!
 * \brief MediaCollection::mediaForId Returns a media object for a row id.
 * \param id
 * \return Returns a media object for a row id.
 */
MediaSource* MediaCollection::mediaForId(qint64 id)
{
  return id_map_.contains(id) ? qobject_cast<MediaSource*>(id_map_[id]) : NULL;
}

/*!
 * \brief MediaCollection::notify_contents_altered
 * \param added
 * \param removed
 */
void MediaCollection::notify_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed)
{
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
      GalleryManager::instance()->database()->get_media_table()->remove(media->get_id());
    }
  }
}

/*!
 * \brief MediaCollection::photoFromFileinfo
 * Returns an existing photo object if we've already loaded one
 * for this file, or NULL otherwise. Used for preventing duplicates
 * from appearing after an edit.
 * \param file_to_load
 * \return
 */
Photo* MediaCollection::photoFromFileinfo(QFileInfo file_to_load)
{
  return file_photo_map_.value(file_to_load.absoluteFilePath(), NULL);
}

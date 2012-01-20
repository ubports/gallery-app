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

const QString MediaCollection::THUMBNAIL_DIR = "thumbs";

MediaCollection* MediaCollection::instance_ = NULL;

MediaCollection::MediaCollection(const QDir& directory)
  : SourceCollection("MediaCollection"), directory_(directory) {
  directory_.setFilter(QDir::Files);
  directory_.setSorting(QDir::Name);
  directory_.mkdir(THUMBNAIL_DIR);
  
  // TODO: Assuming all files are photos in specified directory
  QSet<DataObject*> photos;
  QStringList filenames = directory_.entryList();
  QString filename;
  foreach (filename, filenames) {
    Photo* photo = new Photo(QFileInfo(directory_, filename));
    // TODO: Need to address how to deal with photos that have no exposure
    // date/time.  See also:
    // https://bugs.launchpad.net/goodhope/+bug/918844
    if (!photo->exposure_date().isValid()) {
      delete photo;
      
      continue;
    }
    
    photos.insert(photo);
  }
  
  // By default, sort all media by its exposure date time, ascending
  SetComparator(ExposureDateTimeAscendingComparator);
  
  AddMany(photos);
}

void MediaCollection::InitInstance(const QDir& directory) {
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

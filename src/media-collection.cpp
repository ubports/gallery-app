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

#include "media-collection.h"

#include <QStringList>
#include <QString>

#include "media-object.h"
#include "photo.h"

MediaCollection::MediaCollection(const QDir& directory)
  : directory_(directory) {
  directory_.setFilter(QDir::Files);
  directory_.setSorting(QDir::Name);
  
  // TODO: Assuming all files are photos in specified directory
  QList<DataObject*> photos;
  QStringList filenames = directory_.entryList();
  QString filename;
  foreach (filename, filenames) {
    Photo* photo = new Photo(QFileInfo(directory_, filename));
    photo->Init();
    photos.append(photo);
  }
  
  AddMany(photos);
}

const QDir& MediaCollection::directory() const {
  return directory_;
}

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

#ifndef GALLERY_MEDIA_COLLECTION_H_
#define GALLERY_MEDIA_COLLECTION_H_

#include <QObject>
#include <QDir>

#include "core/data-object.h"
#include "core/source-collection.h"

class MediaCollection : public SourceCollection {
  Q_OBJECT
  
public:
  static void Init(const QDir& directory);
  
  static MediaCollection* instance();
  
  static bool ExposureDateTimeAscendingComparator(DataObject* a, DataObject* b);
  static bool ExposureDateTimeDescendingComparator(DataObject* a, DataObject* b);
  
  const QDir& directory() const;
  
private:
  static MediaCollection* instance_;
  
  QDir directory_;
  
  MediaCollection(const QDir& directory);
};

#endif  // GALLERY_MEDIA_COLLECTION_H_

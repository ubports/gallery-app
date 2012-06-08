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
#include <QMap>
#include <QSet>

#include "core/data-object.h"
#include "core/source-collection.h"
#include "media-source.h"

class MediaCollection : public SourceCollection {
  Q_OBJECT
  
public:
  static void Init(const QDir& directory);
  
  static MediaCollection* instance();
  
  static bool ExposureDateTimeAscendingComparator(DataObject* a, DataObject* b);
  static bool ExposureDateTimeDescendingComparator(DataObject* a, DataObject* b);
  
  const QDir& directory() const;
  
  // Returns a media object for a row id.
  MediaSource* mediaForId(qint64 id);
  
protected slots:
  virtual void notify_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
private:
  static MediaCollection* instance_;
  
  QDir directory_;
  QHash<qint64, DataObject*> id_map_;
  
  MediaCollection(const QDir& directory);
  
};

#endif  // GALLERY_MEDIA_COLLECTION_H_

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
 * Jim Nelson <jim@yorba.org>
 */

#ifndef GALLERY_EVENT_COLLECTION_H_
#define GALLERY_EVENT_COLLECTION_H_

#include <QDate>
#include <QHash>
#include <QSet>

#include "core/source-collection.h"

class DataObject;
class Event;
class MediaSource;

/*!
 * \brief The EventCollection class
 */
class EventCollection : public SourceCollection
{
  Q_OBJECT
  
 signals:
  void all_changed();
  
 public:
  EventCollection();
  
  Event* EventForDate(const QDate& date) const;
  Event* EventForMediaSource(MediaSource* media) const;
  
 protected:
  virtual void notify_contents_altered(const QSet<DataObject *> *added,
    const QSet<DataObject *> *removed);
  
 private slots:
  void on_media_added_removed(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  QHash<QDate, Event*> date_map_;
  
  static bool Comparator(DataObject* a, DataObject* b);
};

#endif  // GALLERY_EVENT_COLLECTION_H_

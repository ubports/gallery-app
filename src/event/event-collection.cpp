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

#include "event/event-collection.h"
#include "event/event.h"
#include "media/media-collection.h"
#include "media/media-source.h"
#include "util/collections.h"
#include "util/time.h"
#include "core/gallery-manager.h"

EventCollection::EventCollection()
  : SourceCollection("EventCollection") {
  SetComparator(Comparator);
  
  // Monitor MediaCollection to create/destroy Events, one for each day of
  // media found
  QObject::connect(
    GalleryManager::GetInstance()->media_collection(),
    SIGNAL(contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_media_added_removed(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  // seed what's already present
  on_media_added_removed(&GalleryManager::GetInstance()->media_collection()->GetAsSet(), NULL);
}

Event* EventCollection::EventForDate(const QDate& date) const {
  return date_map_.value(date);
}

Event* EventCollection::EventForMediaSource(MediaSource* media) const {
  // TODO: Could use lookup table here, but this is fine for now
  Event* event;
  foreach (event, GetAllAsType<Event*>()) {
    if (event->Contains(media))
      return event;
  }
  
  return NULL;
}

// Sorts Events in reverse chronological order
bool EventCollection::Comparator(DataObject* a, DataObject* b) {
  Event* eventa = qobject_cast<Event*>(a);
  Q_ASSERT(eventa != NULL);
  
  Event* eventb = qobject_cast<Event*>(b);
  Q_ASSERT(eventb != NULL);
  
  return eventa->date() > eventb->date();
}

void EventCollection::on_media_added_removed(const QSet<DataObject *> *added,
  const QSet<DataObject *> *removed) {
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added) {
      MediaSource* media = qobject_cast<MediaSource*>(object);
      Q_ASSERT(media != NULL);
      
      Event* existing = date_map_.value(media->exposure_date());
      if (existing == NULL) {
        existing = new Event(this, media->exposure_date());
        
        Add(existing);
      }
      
      existing->Attach(media);
    }
  }
  
  if (removed != NULL) {
    DataObject* object;
    foreach (object, *removed) {
      MediaSource* media = qobject_cast<MediaSource*>(object);
      Q_ASSERT(media != NULL);

      Event* event = date_map_.value(media->exposure_date());
      Q_ASSERT(event != NULL);

      event->Detach(media);

      if (event->ContainedCount() == 0)
        Destroy(event, true, true);
    }
  }
}

void EventCollection::notify_contents_altered(const QSet<DataObject *> *added,
  const QSet<DataObject *> *removed) {
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added) {
      Event* event = qobject_cast<Event*>(object);
      Q_ASSERT(event != NULL);
      
      // One Event per day
      Q_ASSERT(!date_map_.contains(event->date()));
      date_map_.insert(event->date(), event);
    }
  }
  
  if (removed != NULL) {
    DataObject* object;
    foreach (object, *removed) {
      Event* event = qobject_cast<Event*>(object);
      Q_ASSERT(event != NULL);
      
      Q_ASSERT(date_map_.contains(event->date()));
      date_map_.remove(event->date());
    }
  }
  
  SourceCollection::notify_contents_altered(added, removed);
}

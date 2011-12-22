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

#include "source-collection.h"

#include "data-source.h"

SourceCollection::SourceCollection(const QString& name)
  : DataCollection(name) {
}

void SourceCollection::DestroyAll(bool destroy_backing, bool delete_objects) {
  // obtain a copy of all objects
  QSet<DataObject*> all = AsSet();
  
  // must be done before destruction and removal
  notify_destroying(&all);
  
  // remove before destroying
  Clear();
  
  // destroy and optionally delete all objects
  DataObject* object;
  foreach (object, all) {
    DataSource* source = qobject_cast<DataSource*>(object);
    Q_ASSERT(source != NULL);
    
    source->Destroy(destroy_backing);
    if (delete_objects)
      delete source;
  }
}

void SourceCollection::notify_destroying(const QSet<DataObject*>* objects) {
  emit destroying(objects);
}

void SourceCollection::notify_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  if (added != NULL) {
    // set membership of DataSource to this collection
    DataObject* object;
    foreach (object, *added) {
      DataSource *source = qobject_cast<DataSource*>(object);
      source->set_membership(this);
    }
  }
  
  if (removed != NULL) {
    // remove membership of DataSource from this collection
    DataObject* object;
    foreach (object, *removed) {
      DataSource *source = qobject_cast<DataSource*>(object);
      Q_ASSERT(source->member_of() == this);
      source->set_membership(NULL);
    }
  }
  
  DataCollection::notify_contents_altered(added, removed);
}
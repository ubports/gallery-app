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

#include "data-collection.h"

DataCollection::DataCollection() {
}

void DataCollection::notify_added(const QList<DataObject*>& added_objects) {
  emit added(added_objects);
}

int DataCollection::Count() const {
  return list_.size();
}

void DataCollection::Add(DataObject* object) {
  Q_ASSERT(object != NULL);
  
  // TODO: Need to check for double-adds and silently exit
  list_.append(object);
  
  // The "added" signal requires a list as a parameter
  QList<DataObject*> added_list;
  added_list.append(object);
  
  notify_added(added_list);
}

void DataCollection::AddMany(const QList<DataObject*>& objects) {
  if (objects.count() == 0)
    return;
  
  // TODO: Need to check for double-adds and silently exit
  list_.append(objects);
  
  notify_added(objects);
}

const QList<DataObject*>& DataCollection::GetAll() const {
  // TODO: Return a read-only view of the list
  return list_;
}

bool DataCollection::Contains(DataObject* object) const {
  return list_.contains(object);
}

DataObject* DataCollection::GetAt(int index) const {
  return (index < list_.size()) ? list_[index] : NULL;
}

int DataCollection::IndexOf(const DataObject& object) const {
  int size = list_.size();
  for (int index = 0; index < size; index++) {
    if (list_[index] == &object)
      return index;
  }
  
  return -1;
}

DataObject* DataCollection::FindByNumber(DataObjectNumber number) const {
  // TODO: This should be replaced by a hash lookup table
  DataObject* object;
  foreach (object, list_) {
    if (object->number() == number)
      return object;
  }
  
  return NULL;
}

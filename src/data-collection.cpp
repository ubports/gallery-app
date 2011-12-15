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

DataCollection::DataCollection()
  : comparator_(DefaultDataObjectComparator) {
}

void DataCollection::notify_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  emit contents_altered(added, removed);
}

void DataCollection::notify_ordering_altered() {
  emit ordering_altered();
}

int DataCollection::Count() const {
  return list_.size();
}

void DataCollection::Add(DataObject* object) {
  Q_ASSERT(object != NULL);
  
  // TODO: Need to check for double-adds and silently exit
  list_.append(object);
  resort(false);
  
  // The "added" signal requires a list as a parameter
  QSet<DataObject*> added_list;
  added_list.insert(object);
  
  notify_contents_altered(&added_list, NULL);
}

void DataCollection::AddMany(const QSet<DataObject*>& objects) {
  if (objects.count() == 0)
    return;
  
  // TODO: Need to check for double-adds and silently drop
  list_.append(QList<DataObject*>::fromSet(objects));
  resort(false);
  
  notify_contents_altered(&objects, NULL);
}

void DataCollection::Remove(DataObject* object) {
  bool removed = list_.removeOne(object);
  if (!removed)
    return;
  
  QSet<DataObject*> singleton;
  singleton.insert(object);
  
  notify_contents_altered(NULL, &singleton);
}

void DataCollection::RemoveMany(const QSet<DataObject *> &objects) {
  if (objects.count() == 0)
    return;
  
  // Want to only report DataObjects that are actually removed
  QSet<DataObject*> removed;
  
  DataObject* object;
  foreach (object, objects) {
    if (list_.removeOne(object))
      removed.insert(object);
  }
  
  if (removed.count() > 0)
    notify_contents_altered(NULL, &removed);
}

void DataCollection::Clear() {
  if (list_.count() == 0)
    return;
  
  // save copy for signal
  QSet<DataObject*> all = AsSet();
  
  list_.clear();
  
  notify_contents_altered(NULL, &all);
}

const QList<DataObject*>& DataCollection::GetAll() const {
  return list_;
}

const QSet<DataObject*> DataCollection::AsSet() const {
  return QSet<DataObject*>::fromList(list_);
}

bool DataCollection::Contains(DataObject* object) const {
  return list_.contains(object);
}

DataObject* DataCollection::GetAt(int index) const {
  return (index < list_.size()) ? list_[index] : NULL;
}

int DataCollection::IndexOf(DataObject* object) const {
  int size = list_.size();
  for (int index = 0; index < size; index++) {
    if (list_[index] == object)
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

void DataCollection::SetComparator(DataObjectComparator comparator) {
  comparator_ = comparator;
  
  resort(true);
}

DataObjectComparator DataCollection::GetComparator() const {
  return comparator_;
}

bool DataCollection::DefaultDataObjectComparator(DataObject* a, DataObject* b) {
  return a->number() < b->number();
}

// Big honkin' TODO: Much more efficient if items were inserted using
// binary insertion, but this will have to do for now
void DataCollection::resort(bool fire_signal) {
  qSort(list_.begin(), list_.end(), comparator_);
  
  if (fire_signal)
    notify_ordering_altered();
}

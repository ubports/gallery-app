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

DataCollection::DataCollection(const QString& name)
  : name_(name.toUtf8()), comparator_(DefaultDataObjectComparator) {
}

void DataCollection::notify_contents_to_be_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  emit contents_to_be_altered(added, removed);
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
  
  // Silently prevent double-adds
  if (set_.contains(object))
    return;
  
  // The "contents" signals require a QSet as a parameter
  QSet<DataObject*> to_add;
  to_add.insert(object);
  
  notify_contents_to_be_altered(&to_add, NULL);
  
  list_.append(object);
  set_.insert(object);
  Resort(false);
  
  notify_contents_altered(&to_add, NULL);
  
  Sanity();
}

void DataCollection::AddMany(const QSet<DataObject*>& objects) {
  if (objects.count() == 0)
    return;
  
  // Silently prevent double-adds (as well as create a QList for a single
  // append operation on the internal list)
  QList<DataObject*> to_add_list;
  DataObject* object;
  foreach (object, objects) {
    if (!set_.contains(object))
      to_add_list.append(object);
  }
  
  if (to_add_list.count() == 0)
    return;
  
  // The "contents" signals require a QSet as a parameter; also,
  // can use this QSet for the unite() method; but avoid creating it if no
  // objects were removed in above loop (which is quite likely in most use
  // cases)
  QSet<DataObject*> to_add(
    (objects.count() == to_add_list.count()) ? objects : to_add_list.toSet());
  
  notify_contents_to_be_altered(&to_add, NULL);
  
  list_.append(to_add_list);
  set_.unite(to_add);
  Resort(false);
  
  notify_contents_altered(&to_add, NULL);
  
  Sanity();
}

void DataCollection::Remove(DataObject* object) {
  // Silently exit on bad removes
  if (!set_.contains(object))
    return;
  
  // "contents" signals require a QSet as a parameter
  QSet<DataObject*> to_remove;
  to_remove.insert(object);
  
  notify_contents_to_be_altered(NULL, &to_remove);
  
  bool removed = set_.remove(object);
  Q_ASSERT(removed);
  removed = list_.removeOne(object);
  Q_ASSERT(removed);
  
  notify_contents_altered(NULL, &to_remove);
  
  Sanity();
}

void DataCollection::RemoveMany(const QSet<DataObject *> &objects) {
  if (objects.count() == 0)
    return;
  
  // Want to only report DataObjects that are actually removed
  QSet<DataObject*> to_remove;
  DataObject* object;
  foreach (object, objects) {
    if (set_.contains(object))
      to_remove.insert(object);
  }
  
  if (to_remove.count() == 0)
    return;
  
  notify_contents_to_be_altered(NULL, &to_remove);
  
  foreach (object, to_remove) {
    bool removed = list_.removeOne(object);
    Q_ASSERT(removed);
  }
  
  set_.subtract(to_remove);
  
  notify_contents_altered(NULL, &to_remove);
  
  Sanity();
}

void DataCollection::Clear() {
  if (list_.count() == 0) {
    Q_ASSERT(set_.count() == 0);
    
    return;
  }
  
  // save copy for signals
  QSet<DataObject*> all(set_);
  
  notify_contents_to_be_altered(NULL, &all);
  
  list_.clear();
  set_.clear();
  
  notify_contents_altered(NULL, &all);
  
  Sanity();
}

const QList<DataObject*>& DataCollection::GetAll() const {
  return list_;
}

const QSet<DataObject*>& DataCollection::GetAsSet() const {
  return set_;
}

bool DataCollection::Contains(DataObject* object) const {
  return set_.contains(object);
}

DataObject* DataCollection::GetAt(int index) const {
  return (index < list_.size()) ? list_[index] : NULL;
}

int DataCollection::IndexOf(DataObject* object) const {
  if (!set_.contains(object))
    return -1;
  
  int index = list_.indexOf(object);
  
  // Testing with set_ should prevent this possibility
  Q_ASSERT(index >= 0);
  
  return index;
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
  
  Resort(true);
}

DataObjectComparator DataCollection::comparator() const {
  return comparator_;
}

bool DataCollection::DefaultDataObjectComparator(DataObject* a, DataObject* b) {
  return a->number() < b->number();
}

void DataCollection::Sanity() const {
  Q_ASSERT(list_.count() == set_.count());
}

// Big honkin' TODO: Much more efficient if small insertions were performed
// via binary insertion and only quicksorting after large insertions, but this
// will have to do for now
void DataCollection::Resort(bool fire_signal) {
  qSort(list_.begin(), list_.end(), comparator_);
  
  if (fire_signal)
    notify_ordering_altered();
}

void DataCollection::SetInternalName(const QString& name) {
  name_ = name.toUtf8();
}

const char* DataCollection::ToString() const {
  return name_.data();
}

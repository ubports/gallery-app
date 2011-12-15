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

/**
  * A DataCollection is a heavyweight, fully signalled collection class.  It is
  * not intended for general use but rather to hold core data structures that
  * are monitored and widely used throughout the system.  Those data structures
  * must inherit from DataObject, which is the only kind of object DataCollection
  * will hold.
  */

#ifndef GALLERY_DATA_COLLECTION_H_
#define GALLERY_DATA_COLLECTION_H_

#include <QObject>
#include <QList>
#include <QSet>

#include "data-object.h"

// Defined as a LessThan comparator (return true if a is less than b)
typedef bool (*DataObjectComparator)(DataObject* a, DataObject* b);

class DataCollection : public QObject {
  Q_OBJECT
  
 signals:
  // fired *after* the DataObjects have been added to the collection
  void contents_altered(const QSet<DataObject*>* added, const QSet<DataObject*>* removed);
  
  // fired after the the DataCollection has been reordered due to a new
  // DataObjectComparator being installed; if the new comparator doesn't
  // actually affect the ordering, this signal will still be called
  void ordering_altered();
  
 public:
  // Default comparator uses DataObjectNumber
  static bool DefaultDataObjectComparator(DataObject* a, DataObject* b);
  
  DataCollection();
  
  int Count() const;
  
  void Add(DataObject* object);
  void AddMany(const QSet<DataObject*>& objects);
  
  void Remove(DataObject* object);
  void RemoveMany(const QSet<DataObject*>& objects);
  void Clear();
  
  bool Contains(DataObject* object) const;
  const QList<DataObject*>& GetAll() const;
  DataObject* GetAt(int index) const;
  DataObject* FindByNumber(DataObjectNumber number) const;
  int IndexOf(DataObject* media) const;
  
  // Returns a QSet that is a copy of all objects in the DataCollection
  const QSet<DataObject*> AsSet() const;
  
  void SetComparator(DataObjectComparator comparator);
  DataObjectComparator GetComparator() const;
  
 protected:
  virtual void notify_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
  virtual void notify_ordering_altered();
  
 private:
  QList<DataObject*> list_;
  DataObjectComparator comparator_;
  
  void resort(bool fire_signal);
};

#endif  // GALLERY_DATA_COLLECTION_H_

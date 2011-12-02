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

#ifndef GALLERY_DATA_COLLECTION_H_
#define GALLERY_DATA_COLLECTION_H_

#include <QObject>
#include <QList>

#include "data-object.h"

class DataCollection : public QObject {
  Q_OBJECT
  
signals:
  // "added" is fired *after* the DataObjects have been added to the collection
  void added(const QList<DataObject*>& added_objects);
  
public:
  DataCollection();
  
  int Count() const;
  
  void Add(DataObject* object);
  void AddMany(const QList<DataObject*>& objects);
  
  bool Contains(DataObject* object) const;
  const QList<DataObject*>& GetAll() const;
  DataObject* GetAt(int index) const;
  DataObject* FindByNumber(DataObjectNumber number) const;
  int IndexOf(const DataObject& media) const;
  
protected:
  virtual void notify_added(const QList<DataObject*>& added_objects);
  
private:
  QList<DataObject*> list_;
};

#endif  // GALLERY_DATA_COLLECTION_H_

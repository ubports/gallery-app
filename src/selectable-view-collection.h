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
  * SelectableViewCollection adds the notion of selection to a ViewCollection.
  * It's primarily of use in grid or checkerboard views when the user may want
  * to perform an operation on a number of DataSources all at once.
  */

#ifndef GALLERY_SELECTABLE_VIEW_COLLECTION_H_
#define GALLERY_SELECTABLE_VIEW_COLLECTION_H_

#include <QSet>
#include <QList>

#include "view-collection.h"

class SelectableViewCollection : public ViewCollection {
  Q_OBJECT
  
signals:
  void selection_altered(const QSet<DataObject*>* selected,
    const QSet<DataObject*>* unselected);
  
public:
  SelectableViewCollection(const QString& name);
  
  bool IsSelected(DataObject* object) const;
  
  const QSet<DataObject*>& GetSelected() const;
  int GetSelectedCount() const;
  
  // Returns true if the selection state of the DataObject changed, false if
  // already selected or not in collection.
  bool Select(DataObject* object);
  
  // Returns true if the selection state of the DataObject changed, false if
  // already selected or not in collection.
  bool Unselect(DataObject* object);
  
  // Returns false if not in collection.
  bool ToggleSelect(DataObject* object);
  
  // Returns the number of items selected (that weren't selected before)
  int SelectAll();
  
  // Returns the number of items unselected (that weren't unselected before)
  int UnselectAll();
  
protected:
  virtual void notify_selection_altered(QSet<DataObject*>* selected,
    QSet<DataObject*>* unselected);
  
private:
  QSet<DataObject*> selected_;
};

#endif  // GALLERY_SELECTABLE_VIEW_COLLECTION_H_

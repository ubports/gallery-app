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

#include <QObject>
#include <QSet>
#include <QList>

#include "core/data-object.h"
#include "core/view-collection.h"
#include "util/collections.h"

class SelectableViewCollection : public ViewCollection {
  Q_OBJECT
  
 signals:
  void selection_altered(const QSet<DataObject*>* selected,
    const QSet<DataObject*>* unselected);
  
 public:
  SelectableViewCollection(const QString& name);
  
  bool IsSelected(DataObject* object) const;
  
  int SelectedCount() const;
  const QSet<DataObject*>& GetSelected() const;
  
  template <class T>
  QSet<T> GetSelectedAsType() const {
    return CastSetToType<DataObject*, T>(GetSelected());
  }
  
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
  
  // Returns the number of items selected (that weren't selected before)
  int SelectMany(const QSet<DataObject*>& select);
  
  // Returns the number of items unselected (that weren't unselected before)
  int UnselectAll();
  
  // Returns the number of items unselected (that weren't unselected before)
  int UnselectMany(const QSet<DataObject*>& unselect);
  
  // One SelectableViewCollection may monitor the selection status of another ...
  // this does *not* mirror the collection, merely alter selection state of
  // elements in this collection as they change in another
  void MonitorSelectionState(SelectableViewCollection* view);
  void StopMonitoringSelectionState();
  bool isMonitoringSelectionState();
  
 protected:
  virtual void notify_contents_to_be_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  virtual void notify_selection_altered(QSet<DataObject*>* selected,
    QSet<DataObject*>* unselected);
  
 private slots:
  void on_monitoring_selection_altered(const QSet<DataObject*>* selected,
    const QSet<DataObject*>* unselected);
  
 private:
  QSet<DataObject*> selected_;
  SelectableViewCollection* monitoring_selection_;
};

#endif  // GALLERY_SELECTABLE_VIEW_COLLECTION_H_

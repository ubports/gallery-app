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

#include "selectable-view-collection.h"

SelectableViewCollection::SelectableViewCollection() {
}

void SelectableViewCollection::notify_selection_altered(QList<DataObject*>* selected,
  QList<DataObject*>* unselected) {
  emit selection_altered(selected, unselected);
}

bool SelectableViewCollection::IsSelected(DataObject* object) const {
  return selected_.contains(object);
}

bool SelectableViewCollection::Select(DataObject* object) {
  if (!Contains(object) || selected_.contains(object))
    return false;
  
  selected_.insert(object);
  
  QList<DataObject*> singleton;
  singleton.append(object);
  
  notify_selection_altered(&singleton, NULL);
  
  return true;
}

bool SelectableViewCollection::Unselect(DataObject* object) {
  if (!Contains(object) || !selected_.contains(object))
    return false;
  
  bool removed = selected_.remove(object);
  Q_ASSERT(removed);
  
  QList<DataObject*> singleton;
  singleton.append(object);
  
  notify_selection_altered(NULL, &singleton);
  
  return true;
}

bool SelectableViewCollection::ToggleSelect(DataObject* object) {
  return IsSelected(object) ? Unselect(object) : Select(object);
}

int SelectableViewCollection::SelectAll() {
  // Only select objects not already selected
  QList<DataObject*> selected;
  DataObject* object;
  foreach (object, GetAll()) {
    if (!IsSelected(object)) {
      selected_.insert(object);
      selected.append(object);
    }
  }
  
  if (selected.count() > 0)
    notify_selection_altered(&selected, NULL);
  
  return selected.count();
}

int SelectableViewCollection::UnselectAll() {
  if (selected_.count() == 0)
    return 0;
  
  // save all that were selected to a list for the signal
  QList<DataObject*> unselected;
  unselected.append(QList<DataObject*>::fromSet(selected_));
  
  selected_.clear();
  
  notify_selection_altered(NULL, &unselected);
  
  return unselected.count();
}

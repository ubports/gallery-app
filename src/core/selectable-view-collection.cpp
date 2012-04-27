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

#include "core/selectable-view-collection.h"

SelectableViewCollection::SelectableViewCollection(const QString& name)
  : ViewCollection(name), monitoring_selection_(NULL) {
}

void SelectableViewCollection::notify_selection_altered(QSet<DataObject*>* selected,
  QSet<DataObject*>* unselected) {
  emit selection_altered(selected, unselected);
}

bool SelectableViewCollection::IsSelected(DataObject* object) const {
  return selected_.contains(object);
}

const QSet<DataObject*>& SelectableViewCollection::GetSelected() const {
  return selected_;
}

int SelectableViewCollection::SelectedCount() const {
  return selected_.count();
}

bool SelectableViewCollection::Select(DataObject* object) {
  if (!Contains(object) || selected_.contains(object))
    return false;
  
  selected_.insert(object);
  
  QSet<DataObject*> singleton;
  singleton.insert(object);
  
  notify_selection_altered(&singleton, NULL);
  
  return true;
}

bool SelectableViewCollection::Unselect(DataObject* object) {
  if (!Contains(object) || !selected_.contains(object))
    return false;
  
  bool removed = selected_.remove(object);
  Q_ASSERT(removed);
  
  QSet<DataObject*> singleton;
  singleton.insert(object);
  
  notify_selection_altered(NULL, &singleton);
  
  return true;
}

bool SelectableViewCollection::ToggleSelect(DataObject* object) {
  return IsSelected(object) ? Unselect(object) : Select(object);
}

int SelectableViewCollection::SelectAll() {
  return SelectMany(GetAsSet());
}

int SelectableViewCollection::SelectMany(const QSet<DataObject*>& select) {
  // Only select objects not already selected
  QSet<DataObject*> selected;
  DataObject* object;
  foreach (object, select) {
    if (Contains(object) && !IsSelected(object)) {
      selected_.insert(object);
      selected.insert(object);
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
  QSet<DataObject*> unselected;
  unselected.unite(selected_);
  
  selected_.clear();
  
  notify_selection_altered(NULL, &unselected);
  
  return unselected.count();
}

int SelectableViewCollection::UnselectMany(const QSet<DataObject*>& unselect) {
  QSet<DataObject*> unselected;
  DataObject* object;
  foreach (object, unselect) {
    if (Contains(object) && IsSelected(object)) {
      selected_.remove(object);
      unselected.insert(object);
    }
  }
  
  if (unselected.count() > 0)
    notify_selection_altered(NULL, &unselected);
  
  return unselected.count();
}

void SelectableViewCollection::MonitorSelectionState(SelectableViewCollection* view) {
  StopMonitoringSelectionState();
  
  monitoring_selection_ = view;
  
  QObject::connect(monitoring_selection_,
    SIGNAL(selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_monitoring_selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
}

void SelectableViewCollection::StopMonitoringSelectionState() {
  if (monitoring_selection_ == NULL)
    return;
  
  monitoring_selection_ = NULL;
  
  QObject::disconnect(monitoring_selection_,
    SIGNAL(selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_monitoring_selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
}

bool SelectableViewCollection::isMonitoringSelectionState() {
  return (monitoring_selection_ != NULL);
}

void SelectableViewCollection::on_monitoring_selection_altered(
  const QSet<DataObject*>* selected, const QSet<DataObject*>* unselected) {
  if (selected != NULL)
    SelectMany(*selected);
  
  if (unselected != NULL)
    UnselectMany(*unselected);
}

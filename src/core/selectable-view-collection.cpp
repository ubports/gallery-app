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

/*!
 * \brief SelectableViewCollection::SelectableViewCollection
 * \param name
 */
SelectableViewCollection::SelectableViewCollection(const QString& name)
  : ViewCollection(name), monitoring_selection_(NULL)
{
}

/*!
 * \brief SelectableViewCollection::notify_contents_to_be_altered
 * \param added
 * \param removed
 */
void SelectableViewCollection::notify_contents_to_be_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed)
{
  if (removed != NULL)
    UnselectMany(*removed);

  ViewCollection::notify_contents_to_be_altered(added, removed);
}

/*!
 * \brief SelectableViewCollection::notify_selection_altered
 * \param selected
 * \param unselected
 */
void SelectableViewCollection::notify_selection_altered(QSet<DataObject*>* selected,
  QSet<DataObject*>* unselected)
{
  emit selection_altered(selected, unselected);
}

/*!
 * \brief SelectableViewCollection::IsSelected
 * \param object
 * \return
 */
bool SelectableViewCollection::IsSelected(DataObject* object) const
{
  return selected_.contains(object);
}

/*!
 * \brief SelectableViewCollection::GetSelected
 * \return
 */
const QSet<DataObject*>& SelectableViewCollection::GetSelected() const
{
  return selected_;
}

/*!
 * \brief SelectableViewCollection::SelectedCount
 * \return
 */
int SelectableViewCollection::SelectedCount() const
{
  return selected_.count();
}

/*!
 * \brief SelectableViewCollection::Select
 * \param object
 * \return Returns true if the selection state of the DataObject changed, false if already
 * selected or not in collection.
 */
bool SelectableViewCollection::Select(DataObject* object)
{
  if (!Contains(object) || selected_.contains(object))
    return false;
  
  selected_.insert(object);
  
  QSet<DataObject*> singleton;
  singleton.insert(object);
  
  notify_selection_altered(&singleton, NULL);
  
  return true;
}

/*!
 * \brief SelectableViewCollection::Unselect
 * \param object
 * \return Returns true if the selection state of the DataObject changed, false if already selected
 * or not in collection.
 */
bool SelectableViewCollection::Unselect(DataObject* object)
{
  if (!Contains(object) || !selected_.contains(object))
    return false;
  
  bool removed = selected_.remove(object);
  Q_ASSERT(removed);
  Q_UNUSED(removed);

  QSet<DataObject*> singleton;
  singleton.insert(object);
  
  notify_selection_altered(NULL, &singleton);
  
  return true;
}

/*!
 * \brief SelectableViewCollection::ToggleSelect
 * \param object
 * \return Returns false if not in collection
 */
bool SelectableViewCollection::ToggleSelect(DataObject* object)
{
  return IsSelected(object) ? Unselect(object) : Select(object);
}

/*!
 * \brief SelectableViewCollection::SelectAll
 * \return Returns the number of items selected (that weren't selected before)
 */
int SelectableViewCollection::SelectAll()
{
  return SelectMany(GetAsSet());
}

/*!
 * \brief SelectableViewCollection::SelectMany
 * \param select
 * \return Returns the number of items selected (that weren't selected before)
 */
int SelectableViewCollection::SelectMany(const QSet<DataObject*>& select)
{
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

/*!
 * \brief SelectableViewCollection::UnselectAll
 * \return Returns the number of items unselected (that weren't unselected before)
 */
int SelectableViewCollection::UnselectAll()
{
  if (selected_.count() == 0)
    return 0;
  
  // save all that were selected to a list for the signal
  QSet<DataObject*> unselected;
  unselected.unite(selected_);
  
  selected_.clear();
  
  notify_selection_altered(NULL, &unselected);
  
  return unselected.count();
}

/*!
 * \brief SelectableViewCollection::UnselectMany
 * \param unselect
 * \return Returns the number of items unselected (that weren't unselected before)
 */
int SelectableViewCollection::UnselectMany(const QSet<DataObject*>& unselect)
{
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

/*!
 * \brief SelectableViewCollection::MonitorSelectionState
 * \param view
 */
void SelectableViewCollection::MonitorSelectionState(SelectableViewCollection* view)
{
  StopMonitoringSelectionState();

  if (view == NULL)
    return;

  monitoring_selection_ = view;

  QObject::connect(monitoring_selection_,
    SIGNAL(selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_monitoring_selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
}

/*!
 * \brief SelectableViewCollection::StopMonitoringSelectionState
 */
void SelectableViewCollection::StopMonitoringSelectionState()
{
  if (monitoring_selection_ == NULL)
    return;

  QObject::disconnect(monitoring_selection_,
    SIGNAL(selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_monitoring_selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));

  monitoring_selection_ = NULL;
}

/*!
 * \brief SelectableViewCollection::isMonitoringSelectionState
 * \return
 */
bool SelectableViewCollection::isMonitoringSelectionState()
{
  return (monitoring_selection_ != NULL);
}

/*!
 * \brief SelectableViewCollection::on_monitoring_selection_altered
 * \param selected
 * \param unselected
 */
void SelectableViewCollection::on_monitoring_selection_altered(
  const QSet<DataObject*>* selected, const QSet<DataObject*>* unselected)
{
  if (selected != NULL)
    SelectMany(*selected);

  if (unselected != NULL)
    UnselectMany(*unselected);
}

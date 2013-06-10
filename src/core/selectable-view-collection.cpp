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

/*!
 * \brief SelectableViewCollection::SelectableViewCollection
 * \param name
 */
SelectableViewCollection::SelectableViewCollection(const QString& name)
    : ViewCollection(name), m_monitoringSelection(NULL)
{
}

/*!
 * \brief SelectableViewCollection::notifyContentsToBeChanged
 * \param added
 * \param removed
 */
void SelectableViewCollection::notifyContentsToBeChanged(const QSet<DataObject*>* added,
                                                             const QSet<DataObject*>* removed)
{
    if (removed != NULL)
        unselectMany(*removed);

    ViewCollection::notifyContentsToBeChanged(added, removed);
}

/*!
 * \brief SelectableViewCollection::notifySelectionChanged
 * \param selected
 * \param unselected
 */
void SelectableViewCollection::notifySelectionChanged(QSet<DataObject*>* selected,
                                                        QSet<DataObject*>* unselected)
{
    emit selectionChanged(selected, unselected);
}

/*!
 * \brief SelectableViewCollection::isSelected
 * \param object
 * \return
 */
bool SelectableViewCollection::isSelected(DataObject* object) const
{
    return m_selected.contains(object);
}

/*!
 * \brief SelectableViewCollection::getSelected
 * \return
 */
const QSet<DataObject*>& SelectableViewCollection::getSelected() const
{
    return m_selected;
}

/*!
 * \brief SelectableViewCollection::selectedCount
 * \return
 */
int SelectableViewCollection::selectedCount() const
{
    return m_selected.count();
}

/*!
 * \brief SelectableViewCollection::select
 * \param object
 * \return Returns true if the selection state of the DataObject changed, false if already
 * selected or not in collection.
 */
bool SelectableViewCollection::select(DataObject* object)
{
    if (!contains(object) || m_selected.contains(object))
        return false;

    m_selected.insert(object);

    QSet<DataObject*> singleton;
    singleton.insert(object);

    notifySelectionChanged(&singleton, NULL);

    return true;
}

/*!
 * \brief SelectableViewCollection::unselect
 * \param object
 * \return Returns true if the selection state of the DataObject changed, false if already selected
 * or not in collection.
 */
bool SelectableViewCollection::unselect(DataObject* object)
{
    if (!contains(object) || !m_selected.contains(object))
        return false;

    bool removed = m_selected.remove(object);
    Q_ASSERT(removed);
    Q_UNUSED(removed);

    QSet<DataObject*> singleton;
    singleton.insert(object);

    notifySelectionChanged(NULL, &singleton);

    return true;
}

/*!
 * \brief SelectableViewCollection::toggleSelect
 * \param object
 * \return Returns false if not in collection
 */
bool SelectableViewCollection::toggleSelect(DataObject* object)
{
    return isSelected(object) ? unselect(object) : select(object);
}

/*!
 * \brief SelectableViewCollection::selectAll
 * \return Returns the number of items selected (that weren't selected before)
 */
int SelectableViewCollection::selectAll()
{
    return selectMany(getAsSet());
}

/*!
 * \brief SelectableViewCollection::selectMany
 * \param select
 * \return Returns the number of items selected (that weren't selected before)
 */
int SelectableViewCollection::selectMany(const QSet<DataObject*>& select)
{
    // Only select objects not already selected
    QSet<DataObject*> selected;
    DataObject* object;
    foreach (object, select) {
        if (contains(object) && !isSelected(object)) {
            m_selected.insert(object);
            selected.insert(object);
        }
    }

    if (selected.count() > 0)
        notifySelectionChanged(&selected, NULL);

    return selected.count();
}

/*!
 * \brief SelectableViewCollection::unselectAll
 * \return Returns the number of items unselected (that weren't unselected before)
 */
int SelectableViewCollection::unselectAll()
{
    if (m_selected.count() == 0)
        return 0;

    // save all that were selected to a list for the signal
    QSet<DataObject*> unselected;
    unselected.unite(m_selected);

    m_selected.clear();

    notifySelectionChanged(NULL, &unselected);

    return unselected.count();
}

/*!
 * \brief SelectableViewCollection::unselectMany
 * \param unselect
 * \return Returns the number of items unselected (that weren't unselected before)
 */
int SelectableViewCollection::unselectMany(const QSet<DataObject*>& unselect)
{
    QSet<DataObject*> unselected;
    DataObject* object;
    foreach (object, unselect) {
        if (contains(object) && isSelected(object)) {
            m_selected.remove(object);
            unselected.insert(object);
        }
    }

    if (unselected.count() > 0)
        notifySelectionChanged(NULL, &unselected);

    return unselected.count();
}

/*!
 * \brief SelectableViewCollection::monitorSelectionState
 * \param view
 */
void SelectableViewCollection::monitorSelectionState(SelectableViewCollection* view)
{
    stopMonitoringSelectionState();

    if (view == NULL)
        return;

    m_monitoringSelection = view;

    QObject::connect(m_monitoringSelection,
                     SIGNAL(selectionChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)),
                     this,
                     SLOT(onMonitoringSelectionChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)));
}

/*!
 * \brief SelectableViewCollection::stopMonitoringSelectionState
 */
void SelectableViewCollection::stopMonitoringSelectionState()
{
    if (m_monitoringSelection == NULL)
        return;

    QObject::disconnect(m_monitoringSelection,
                        SIGNAL(selectionChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)),
                        this,
                        SLOT(onMonitoringSelectionChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)));

    m_monitoringSelection = NULL;
}

/*!
 * \brief SelectableViewCollection::isMonitoringSelectionState
 * \return
 */
bool SelectableViewCollection::isMonitoringSelectionState()
{
    return (m_monitoringSelection != NULL);
}

/*!
 * \brief SelectableViewCollection::onMonitoringSelectionChanged
 * \param selected
 * \param unselected
 */
void SelectableViewCollection::onMonitoringSelectionChanged(
        const QSet<DataObject*>* selected, const QSet<DataObject*>* unselected)
{
    if (selected != NULL)
        selectMany(*selected);

    if (unselected != NULL)
        unselectMany(*unselected);
}

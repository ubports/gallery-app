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

#ifndef GALLERY_SELECTABLE_VIEW_COLLECTION_H_
#define GALLERY_SELECTABLE_VIEW_COLLECTION_H_

#include "data-object.h"
#include "view-collection.h"

#include <QSet>

/**
  * SelectableViewCollection adds the notion of selection to a ViewCollection.
  * It's primarily of use in grid or checkerboard views when the user may want
  * to perform an operation on a number of DataSources all at once.
  */
class SelectableViewCollection : public ViewCollection
{
    Q_OBJECT

signals:
    void selectionChanged(const QSet<DataObject*>* selected,
                          const QSet<DataObject*>* unselected);

public:
    SelectableViewCollection(const QString& name);

    bool isSelected(DataObject* object) const;

    int selectedCount() const;
    const QSet<DataObject*>& getSelected() const;

    template <class T>
    QSet<T> getSelectedAsType() const {
        return CastSetToType<DataObject*, T>(getSelected());
    }

    bool select(DataObject* object);
    bool unselect(DataObject* object);
    bool toggleSelect(DataObject* object);
    int selectAll();
    int selectMany(const QSet<DataObject*>& select);
    int unselectAll();
    int unselectMany(const QSet<DataObject*>& unselect);

    // One SelectableViewCollection may monitor the selection status of another ...
    // this does *not* mirror the collection, merely alter selection state of
    // elements in this collection as they change in another
    void monitorSelectionState(SelectableViewCollection* view);
    void stopMonitoringSelectionState();
    bool isMonitoringSelectionState();

protected:
    virtual void notifyContentsToBeChanged(const QSet<DataObject*>* added,
                                               const QSet<DataObject*>* removed);
    virtual void notifySelectionChanged(QSet<DataObject*>* selected,
                                          QSet<DataObject*>* unselected);

private slots:
    void onMonitoringSelectionChanged(const QSet<DataObject*>* selected,
                                         const QSet<DataObject*>* unselected);

private:
    QSet<DataObject*> m_selected;
    SelectableViewCollection* m_monitoringSelection;
};

#endif  // GALLERY_SELECTABLE_VIEW_COLLECTION_H_

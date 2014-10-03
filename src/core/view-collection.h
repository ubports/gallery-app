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

#ifndef GALLERY_VIEW_COLLECTION_H_
#define GALLERY_VIEW_COLLECTION_H_

#include "data-collection.h"
#include "source-collection.h"

class IDataFilter {
public:
    virtual bool isAccepted(DataObject* item) = 0;
};

/**
  * A ViewCollection is the logical complement to a SourceCollection.  Where
  * there is only one SourceCollection for each finalized DataSource type,
  * there may be many ViewCollections that hold collections of DataSources
  * for some organizational reason, be it to present to the user or internally
  * by a ContainerSource to represent all the DataSources it organizes.
  *
  * ViewCollections may monitor other DataCollections, meaning that as
  * DataObjects are added, removed, or altered in a DataCollection these
  * changes are represented in the ViewCollection as well.  This allows for a
  * ViewCollection to maintain a coherent representation of DataObject filtered
  * by a predicate function, and even maintaining the same sort ordering.
  *
  * The "view" in ViewCollection should not be thought of in the sense of
  * model-view-controller, although there are some similarities.  Rather, it
  * should be thought of a table view in database parlance -- a slice of a
  * larger table that maintains coherence as the larger table mutates.
  */
class ViewCollection : public DataCollection
{
    Q_OBJECT

    Q_PROPERTY(const DataCollection* collection READ collection NOTIFY collectionChanged)

public:
    ViewCollection(const QString& name);

    // TODO: Allow multiple SourceCollections to be monitored.  Without a
    // DataView as a mediator, this means ViewCollection (and, hence,
    // DataCollection) will hold DataSources of varied finalized types.
    void monitorDataCollection(const DataCollection* collection, IDataFilter* filter,
                               bool monitor_ordering);
    bool isMonitoring() const;
    const DataCollection* collection() const;

signals:
    void collectionChanged();

protected:
    virtual void notifyOrderingChanged();

private slots:
    void onMonitoredContentsChanged(const QSet<DataObject*>* added,
                                    const QSet<DataObject*>* removed,
                                    bool notify);
    void onMonitoredOrderingChanged();

private:
    const DataCollection* m_monitoring;
    IDataFilter* m_monitorFilter;
    bool m_monitorOrdering;
};

#endif  // GALLERY_VIEW_COLLECTION_H_

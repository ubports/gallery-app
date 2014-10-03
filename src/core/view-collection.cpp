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

#include "view-collection.h"

/*!
 * \brief ViewCollection::ViewCollection
 * \param name
 */
ViewCollection::ViewCollection(const QString& name)
    : DataCollection(name), m_monitoring(NULL), m_monitorFilter(NULL), m_monitorOrdering(false)
{
}

/*!
 * \brief ViewCollection::monitorDataCollection
 * \param collection
 * \param filter
 * \param monitor_ordering
 */
void ViewCollection::monitorDataCollection(const DataCollection* collection,
                                           IDataFilter* filter, bool monitor_ordering)
{
    Q_ASSERT(collection != NULL);

    // TODO: Allow for monitoring to be halted
    Q_ASSERT(m_monitoring == NULL);

    m_monitoring = collection;
    m_monitorFilter = filter;
    m_monitorOrdering = monitor_ordering;

    // monitor DataCollection for added/removed DataObjects and add them to this
    // ViewCollection according to the filter
    QObject::connect(m_monitoring,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)),
                     this,
                     SLOT(onMonitoredContentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)));

    // If monitoring the ordering, prime the local comparator with the monitored
    // and make sure it's continually reflected
    if (m_monitorOrdering) {
        setComparator(m_monitoring->comparator());

        QObject::connect(m_monitoring, SIGNAL(orderingChanged()), this,
                         SLOT(onMonitoredOrderingChanged()));
    }

    // prime the local ViewCollection with what's already in the monitored
    // DataCollection
    QSet<DataObject*> all(collection->getAsSet());
    onMonitoredContentsChanged(&all, NULL, true);
    emit collectionChanged();
}

/*!
 * \brief ViewCollection::collection
 * \return
 */
const DataCollection* ViewCollection::collection() const
{
    return m_monitoring;
}

/*!
 * \brief ViewCollection::isMonitoring
 * \return
 */
bool ViewCollection::isMonitoring() const
{
    return m_monitoring != NULL;
}

/*!
 * \brief ViewCollection::notifyOrderingChanged
 */
void ViewCollection::notifyOrderingChanged()
{
    if (m_monitorOrdering)
        qWarning("ViewCollection monitoring a DataCollection's ordering changed "
                 "its own comparator: this is unstable");

    DataCollection::notifyOrderingChanged();
}

/*!
 * \brief ViewCollection::onMonitoredContentsChanged
 * \param added
 * \param removed
 */
void ViewCollection::onMonitoredContentsChanged(const QSet<DataObject*>* added,
                                                   const QSet<DataObject*>* removed,
                                                   bool notify)
{
    if (added != NULL) {
        // if no filter, add everything, otherwise run everything through the filter
        if (m_monitorFilter == NULL) {
            addMany(*added);
        } else {
            QSet<DataObject*> to_add;
            DataObject* object;
            foreach (object, *added) {
                if (m_monitorFilter->isAccepted(object))
                    to_add.insert(object);
            }

            addMany(to_add);
        }
    }

    if (removed != NULL)
        removeMany(*removed, notify);
}

/*!
 * \brief ViewCollection::onMonitoredOrderingChanged
 */
void ViewCollection::onMonitoredOrderingChanged()
{
    // simply re-sort the local collection with the monitored collection's new
    // comparator
    setComparator(m_monitoring->comparator());
}

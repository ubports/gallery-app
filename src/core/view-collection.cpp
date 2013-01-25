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

#include "core/view-collection.h"

/*!
 * \brief ViewCollection::ViewCollection
 * \param name
 */
ViewCollection::ViewCollection(const QString& name)
  : DataCollection(name), monitoring_(NULL), monitor_filter_(NULL), monitor_ordering_(false)
{
}

/*!
 * \brief ViewCollection::MonitorDataCollection
 * \param collection
 * \param filter
 * \param monitor_ordering
 */
void ViewCollection::MonitorDataCollection(const DataCollection* collection,
  SourceFilter filter, bool monitor_ordering)
{
  Q_ASSERT(collection != NULL);
  
  // TODO: Allow for monitoring to be halted
  Q_ASSERT(monitoring_ == NULL);
  
  monitoring_ = collection;
  monitor_filter_ = filter;
  monitor_ordering_ = monitor_ordering;
  
  // monitor DataCollection for added/removed DataObjects and add them to this
  // ViewCollection according to the filter
  QObject::connect(monitoring_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_monitored_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  // If monitoring the ordering, prime the local comparator with the monitored
  // and make sure it's continually reflected
  if (monitor_ordering_) {
    SetComparator(monitoring_->comparator());
    
    QObject::connect(monitoring_, SIGNAL(ordering_altered()), this,
      SLOT(on_monitored_ordering_altered()));
  }
  
  // prime the local ViewCollection with what's already in the monitored
  // DataCollection
  QSet<DataObject*> all(collection->GetAsSet());
  on_monitored_contents_altered(&all, NULL);
}

/*!
 * \brief ViewCollection::IsMonitoring
 * \return
 */
bool ViewCollection::IsMonitoring() const
{
  return monitoring_ != NULL;
}

/*!
 * \brief ViewCollection::notify_ordering_altered
 */
void ViewCollection::notify_ordering_altered()
{
  if (monitor_ordering_)
    qWarning("ViewCollection monitoring a DataCollection's ordering changed "
      "its own comparator: this is unstable");
  
  DataCollection::notify_ordering_altered();
}

/*!
 * \brief ViewCollection::on_monitored_contents_altered
 * \param added
 * \param removed
 */
void ViewCollection::on_monitored_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed)
{
  if (added != NULL) {
    // if no filter, add everything, otherwise run everything through the filter
    if (monitor_filter_ == NULL) {
      AddMany(*added);
    } else {
      QSet<DataObject*> to_add;
      DataObject* object;
      foreach (object, *added) {
        if (monitor_filter_(object))
          to_add.insert(object);
      }
      
      AddMany(to_add);
    }
  }
  
  if (removed != NULL)
    RemoveMany(*removed);
}

/*!
 * \brief ViewCollection::on_monitored_ordering_altered
 */
void ViewCollection::on_monitored_ordering_altered()
{
  // simply re-sort the local collection with the monitored collection's new
  // comparator
  SetComparator(monitoring_->comparator());
}

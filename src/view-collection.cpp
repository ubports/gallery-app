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

ViewCollection::ViewCollection()
  : monitoring_(NULL), monitor_filter_(NULL) {
}

void ViewCollection::MonitorSourceCollection(SourceCollection* sources,
  SourceFilter filter) {
  Q_ASSERT(sources != NULL);
  Q_ASSERT(monitoring_ == NULL);
  
  monitoring_ = sources;
  monitor_filter_ = filter;
  
  // monitor SourceCollection for added/removed DataObjects and add them to this
  // ViewCollection according to the filter
  QObject::connect(monitoring_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_monitored_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
  
  // prime the ViewCollection with what's already in the SourceCollection
  QSet<DataObject*> all = sources->AsSet();
  on_monitored_contents_altered(&all, NULL);
}

void ViewCollection::on_monitored_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
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
}

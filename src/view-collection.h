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
  * A ViewCollection is the logical complement to a SourceCollection.  Where
  * there is only one SourceCollection for each finalized DataSource type,
  * there may be many ViewCollections that hold collections of DataSources
  * for some organizational reason, be it to present to the user or internally
  * by a ContainerSource to represent all the DataSources it organizes.
  *
  * ViewCollections may monitor source collections, meaning that as DataSources
  * are added, removed, or altered in a SourceCollection they are represented
  * in the ViewCollection as well.  This allows for a ViewCollection to always
  * maintain a coherent representation of DataSources filtered by a predicate
  * function.
  *
  * The "view" in ViewCollection should not be thought of in the sense of
  * model-view-controller, although there are some similarities.  Rather, it
  * should be thought of a table view in database parlance -- a slice of a
  * larger table that maintains coherence as the larger table mutates.
  */

#ifndef GALLERY_VIEW_COLLECTION_H_
#define GALLERY_VIEW_COLLECTION_H_

#include "data-collection.h"
#include "source-collection.h"

typedef bool (*SourceFilter)(DataObject* object);

class ViewCollection : public DataCollection {
  Q_OBJECT
  
public:
  ViewCollection();
  
  // TODO: Allow multiple SourceCollections to be monitored.  Without a
  // DataView as a mediator, this means ViewCollection (and, hence,
  // DataCollection) will hold DataSources of varied finalized types.
  void MonitorSourceCollection(SourceCollection* sources, SourceFilter filter);
  
private slots:
  void on_monitored_contents_altered(const QSet<DataObject*>*added,
    const QSet<DataObject*>* removed);
  
private:
  SourceCollection* monitoring_;
  SourceFilter monitor_filter_;
};

#endif  // GALLERY_VIEW_COLLECTION_H_

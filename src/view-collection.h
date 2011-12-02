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

typedef bool (*SourceFilter)(DataObject* object);

class ViewCollection : public DataCollection {
  Q_OBJECT
  
public:
  ViewCollection();
  
  void MonitorSourceCollection(SourceCollection* sources, SourceFilter filter);
  
private slots:
  void on_monitored_sources_added(const QList<DataObject*>& added);
  
private:
  SourceCollection* monitoring_;
  SourceFilter monitor_filter_;
};

#endif  // GALLERY_VIEW_COLLECTION_H_

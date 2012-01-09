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
  * A DataSource represents a primary source of a valued resource, content,
  * or organizational structure in the application.  In general, DataSources
  * are loaded en masse at startup, added and removed carefully during program
  * operation, and go the grave in-memory at shutdown.
  *
  * DataSources do not automatically add themselves to their logical
  * SourceCollection; this would be nice but has performance implications with
  * large libraries, especially at startup.  It is the responsibility of the
  * user to ensure that all DataSources are added to their SourceCollection.
  * In general, it's best to make all DataSources at once and add them in one
  * operation to the SourceCollection.
  *
  * Once added, the SourceCollection will register the DataSource's membership,
  * creating a backpointer from the DataSource to its SourceCollection.  A
  * DataSource may belong to only one SourceCollection but many ViewCollections
  * (which do not register their membership with it).
  *
  * Because destroying a DataSource is a heavyweight operation, they must be
  * manually destroyed via the Destroy() method.  The boolean parameter
  * indicates whether or not the backing data source (file, database entry, etc.)
  * is deleted as well.
  *
  * In general, one DataSource may be represented in various ways throughout
  * the system, but the representations may come and go as the user operates
  * the application.
  */

#ifndef GALLERY_DATA_SOURCE_H_
#define GALLERY_DATA_SOURCE_H_

#include <QObject>

#include "core/data-object.h"
#include "core/source-collection.h"

class DataSource : public DataObject {
  Q_OBJECT
  
signals:
  void destroying(bool destroying_backing);
  void destroyed(bool destroyed_backing);
  
public:
  DataSource(const QString& name);
  
  friend class SourceCollection;
  
  // Returns NULL if not a member of any SourceCollection (i.e. is orphaned).
  SourceCollection* member_of() const;
  
protected:
  virtual void notify_destroying(bool destroying_backing);
  virtual void notify_destroyed(bool destroyed_backing);
  
  // This is only called by SourceCollection.
  virtual void Destroy(bool destroy_backing);
  
  // DataSource subclasses need to implement this by performing clean-up
  // work prior to being removed from the system ... if destroy_backing is
  // true, the backing (file, database row, etc.) should be erased as well.
  virtual void DestroySource(bool destroy_backing) = 0;
  
private:
  SourceCollection *membership_;
  
  // Set to NULL if no longer a member of a DataCollection.  Will assert if
  // membership to a non-null DataCollection is set while already a member of
  // another collection.
  void set_membership(SourceCollection* collection);
};

#endif  // GALLERY_DATA_SOURCE_H_

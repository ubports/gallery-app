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

#ifndef GALLERY_DATA_SOURCE_H_
#define GALLERY_DATA_SOURCE_H_

#include "data-object.h"

class SourceCollection;

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
class DataSource : public DataObject
{
    Q_OBJECT

signals:
    void destroying(bool destroying_backing, bool as_orphan);
    void destroyed(bool destroyed_backing, bool as_orphan);

public:
    DataSource(QObject * parent = 0);

    friend class SourceCollection;

    SourceCollection* memberOf() const;

    virtual void destroyOrphan(bool destroy_backing);

protected:
    virtual void notifyDestroying(bool destroying_backing, bool as_orphan);
    virtual void notifyDestroyed(bool destroyed_backing, bool as_orphan);

    virtual void destroy(bool destroy_backing);

    /// DataSource subclasses need to implement this by performing clean-up
    /// work prior to being removed from the system ... if destroy_backing is
    /// true, the backing (file, database row, etc.) should be erased as well.
    virtual void destroySource(bool destroy_backing, bool as_orphan) = 0;

private:
    SourceCollection *m_membership;

    void setMembership(SourceCollection* collection);
};

#endif  // GALLERY_DATA_SOURCE_H_

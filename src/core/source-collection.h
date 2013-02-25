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

#ifndef GALLERY_SOURCE_COLLECTION_H_
#define GALLERY_SOURCE_COLLECTION_H_

#include "core/data-collection.h"

class DataSource;

/**
  * A SourceCollection is a collection of DataSources of a particular finalized
  * type.  In general (but with notable exceptions) a SourceCollection holds
  * every instance of a particular type; thus, there's usually only one instance
  * of a SourceCollection for that type.  For examine, if Photo is a DataSource,
  * there is one SourceCollection that knows about every Photo instantiated in
  * the system.  This is similar in intent (but not in implementation or design)
  * to Smalltalk's allInstances keyword.
  */
class SourceCollection : public DataCollection
{
    Q_OBJECT

signals:
    // Fired before the objects are destroyed and removed from the SourceCollection
    void destroying(const QSet<DataObject*>* objects);

public:
    SourceCollection(const QString& name);

    void DestroyAll(bool destroy_backing, bool delete_objects);
    void DestroyMany(const QSet<DataObject*>& objects, bool destroy_backing,
                     bool delete_objects);
    void Destroy(DataSource* object, bool destroy_backing, bool delete_object);

protected:
    virtual void notify_destroying(const QSet<DataObject*>* objects);

    virtual void notify_contents_altered(const QSet<DataObject*>* added,
                                         const QSet<DataObject*>* removed);

private:
    void DestroyObjects(const QSet<DataObject*>& objects, bool destroy_backing,
                        bool delete_objects);
};

#endif  // GALLERY_SOURCE_COLLECTION_H_

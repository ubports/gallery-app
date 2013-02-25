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

#include "core/source-collection.h"

#include "core/data-source.h"

/*!
 * \brief SourceCollection::SourceCollection
 * \param name
 */
SourceCollection::SourceCollection(const QString& name)
    : DataCollection(name)
{
}

/*!
 * \brief SourceCollection::DestroyAll
 * \param destroy_backing
 * \param delete_objects
 */
void SourceCollection::DestroyAll(bool destroy_backing, bool delete_objects)
{
    if (Count() == 0)
        return;

    // obtain a copy of all objects
    QSet<DataObject*> all(GetAsSet());

    // must be done before destruction and removal
    notify_destroying(&all);

    // remove before destroying
    Clear();

    // destroy and optionally delete all objects
    DestroyObjects(all, destroy_backing, delete_objects);
}

/*!
 * \brief SourceCollection::DestroyMany
 * \param objects
 * \param destroy_backing
 * \param delete_objects
 */
void SourceCollection::DestroyMany(const QSet<DataObject*>& objects,
                                   bool destroy_backing, bool delete_objects)
{
    QSet<DataObject*> intersection(GetAsSet());
    intersection.intersect(objects);

    if (intersection.count() == 0)
        return;

    notify_destroying(&intersection);

    RemoveMany(intersection);

    DestroyObjects(intersection, destroy_backing, delete_objects);
}

/*!
 * \brief SourceCollection::Destroy
 * \param object
 * \param destroy_backing
 * \param delete_object
 */
void SourceCollection::Destroy(DataSource* object, bool destroy_backing,
                               bool delete_object)
{
    if (object == NULL || !Contains(object))
        return;

    // Encapsulate the object in a set for the signals below.
    QSet<DataObject*> object_set;
    object_set.insert(object);

    // must be done before destruction and removal
    notify_destroying(&object_set);

    // remove before destroying
    Remove(object);

    object->Destroy(destroy_backing);
    if (delete_object)
        delete object;
}

/*!
 * \brief SourceCollection::notify_destroying
 * \param objects
 */
void SourceCollection::notify_destroying(const QSet<DataObject*>* objects)
{
    emit destroying(objects);
}

/*!
 * \brief SourceCollection::notify_contents_altered
 * \param added
 * \param removed
 */
void SourceCollection::notify_contents_altered(const QSet<DataObject*>* added,
                                               const QSet<DataObject*>* removed)
{
    if (added != NULL) {
        // set membership of DataSource to this collection
        DataObject* object;
        foreach (object, *added) {
            DataSource *source = qobject_cast<DataSource*>(object);
            source->set_membership(this);
        }
    }

    if (removed != NULL) {
        // remove membership of DataSource from this collection
        DataObject* object;
        foreach (object, *removed) {
            DataSource *source = qobject_cast<DataSource*>(object);
            Q_ASSERT(source->member_of() == this);
            source->set_membership(NULL);
        }
    }

    DataCollection::notify_contents_altered(added, removed);
}

/*!
 * \brief SourceCollection::DestroyObjects
 * \param objects
 * \param destroy_backing
 * \param delete_objects
 */
void SourceCollection::DestroyObjects(const QSet<DataObject*>& objects,
                                      bool destroy_backing, bool delete_objects)
{
    DataObject* object;
    foreach (object, objects) {
        DataSource* source = qobject_cast<DataSource*>(object);
        Q_ASSERT(source != NULL);

        source->Destroy(destroy_backing);
        if (delete_objects)
            delete source;
    }
}

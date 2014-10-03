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

#include "source-collection.h"
#include "data-source.h"

/*!
 * \brief SourceCollection::SourceCollection
 * \param name
 */
SourceCollection::SourceCollection(const QString& name)
    : DataCollection(name)
{
}

/*!
 * \brief SourceCollection::destroyAll
 * \param destroy_backing
 * \param delete_objects
 */
void SourceCollection::destroyAll(bool destroy_backing, bool delete_objects)
{
    if (count() == 0)
        return;

    // obtain a copy of all objects
    QSet<DataObject*> all(getAsSet());

    // must be done before destruction and removal
    notifyDestroying(&all);

    // remove before destroying
    clear();

    // destroy and optionally delete all objects
    destroyObjects(all, destroy_backing, delete_objects);
}

/*!
 * \brief SourceCollection::destroyMany
 * \param objects
 * \param destroy_backing
 * \param delete_objects
 */
void SourceCollection::destroyMany(const QSet<DataObject*>& objects,
                                   bool destroy_backing, bool delete_objects)
{
    QSet<DataObject*> intersection(getAsSet());
    intersection.intersect(objects);

    if (intersection.count() == 0)
        return;

    notifyDestroying(&intersection);

    removeMany(intersection, true);

    destroyObjects(intersection, destroy_backing, delete_objects);
}

/*!
 * \brief SourceCollection::destroy
 * \param object
 * \param destroy_backing
 * \param delete_object
 */
void SourceCollection::destroy(DataSource* object, bool destroy_backing,
                               bool delete_object)
{
    if (object == NULL || !contains(object))
        return;

    // Encapsulate the object in a set for the signals below.
    QSet<DataObject*> object_set;
    object_set.insert(object);

    // must be done before destruction and removal
    notifyDestroying(&object_set);

    // remove before destroying
    remove(object, true);

    object->destroy(destroy_backing);
    if (delete_object)
        delete object;
}

/*!
 * \brief SourceCollection::notifyDestroying
 * \param objects
 */
void SourceCollection::notifyDestroying(const QSet<DataObject*>* objects)
{
    emit destroying(objects);
}

/*!
 * \brief SourceCollection::notifyContentsChanged
 * \param added
 * \param removed
 */
void SourceCollection::notifyContentsChanged(const QSet<DataObject*>* added,
                                               const QSet<DataObject*>* removed,
                                               bool notify)
{
    if (added != NULL) {
        // set membership of DataSource to this collection
        DataObject* object;
        foreach (object, *added) {
            DataSource *source = qobject_cast<DataSource*>(object);
            source->setMembership(this);
        }
    }

    if (removed != NULL) {
        // remove membership of DataSource from this collection
        DataObject* object;
        foreach (object, *removed) {
            DataSource *source = qobject_cast<DataSource*>(object);
            Q_ASSERT(source->memberOf() == this);
            source->setMembership(NULL);
        }
    }

    DataCollection::notifyContentsChanged(added, removed, true);
}

/*!
 * \brief SourceCollection::destroyObjects
 * \param objects
 * \param destroy_backing
 * \param delete_objects
 */
void SourceCollection::destroyObjects(const QSet<DataObject*>& objects,
                                      bool destroy_backing, bool delete_objects)
{
    DataObject* object;
    foreach (object, objects) {
        DataSource* source = qobject_cast<DataSource*>(object);
        Q_ASSERT(source != NULL);

        source->destroy(destroy_backing);
        if (delete_objects)
            delete source;
    }
}

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

#include <QQmlEngine>

#include "data-collection.h"
#include "core/data-object.h"
#include "gallery-application.h"

/*!
 * \brief DataCollection::DataCollection
 * \param name
 */
DataCollection::DataCollection(const QString& name)
    : name_(name.toUtf8()), comparator_(DefaultDataObjectComparator)
{
    // All DataCollections are registered as C++ ownership; QML should never GC them
    GalleryApplication::instance()->setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/*!
 * \brief DataCollection::notify_contents_to_be_altered
 * \param added
 * \param removed
 */
void DataCollection::notify_contents_to_be_altered(const QSet<DataObject*>* added,
                                                   const QSet<DataObject*>* removed)
{
    emit contents_to_be_altered(added, removed);
}

/*!
 * \brief DataCollection::notify_contents_altered
 * \param added
 * \param removed
 */
void DataCollection::notify_contents_altered(const QSet<DataObject*>* added,
                                             const QSet<DataObject*>* removed)
{
    emit contents_altered(added, removed);
}

/*!
 * \brief DataCollection::notify_ordering_altered
 */
void DataCollection::notify_ordering_altered()
{
    emit ordering_altered();
}

/*!
 * \brief DataCollection::Count
 * \return
 */
int DataCollection::Count() const
{
    return list_.size();
}

/*!
 * \brief DataCollection::Add
 * \param object
 */
void DataCollection::Add(DataObject* object)
{
    Q_ASSERT(object != NULL);

    // Silently prevent double-adds
    if (set_.contains(object))
        return;

    // The "contents" signals require a QSet as a parameter
    QSet<DataObject*> to_add;
    to_add.insert(object);

    notify_contents_to_be_altered(&to_add, NULL);

    // Cheaper to binary insert single item than append it to list and do a
    // complete re-sort
    BinaryListInsert(object);
    set_.insert(object);

    notify_contents_altered(&to_add, NULL);

    Sanity();
}

/*!
 * \brief DataCollection::AddMany
 * \param objects
 */
void DataCollection::AddMany(const QSet<DataObject*>& objects)
{
    if (objects.count() == 0)
        return;

    // Silently prevent double-adds (as well as create a QList for a single
    // append operation on the internal list)
    QList<DataObject*> to_add_list;
    DataObject* object;
    foreach (object, objects) {
        Q_ASSERT(object != NULL);

        if (!set_.contains(object))
            to_add_list.append(object);
    }

    int add_count = to_add_list.count();
    if (add_count == 0)
        return;

    // The "contents" signals require a QSet as a parameter, however, avoid
    // creating it if no objects were removed in above loop (which is quite likely
    // in most use cases)
    QSet<DataObject*> to_add(
                (objects.count() == add_count) ? objects : to_add_list.toSet());

    notify_contents_to_be_altered(&to_add, NULL);

    // use binary insertion if only adding one object
    if (add_count == 1) {
        object = to_add_list[0];

        BinaryListInsert(object);
        set_.insert(object);
    } else {
        list_.append(to_add_list);
        set_.unite(to_add);

        Resort(false);
    }

    notify_contents_altered(&to_add, NULL);

    Sanity();
}

/*!
 * \brief DataCollection::Remove
 * \param object
 */
void DataCollection::Remove(DataObject* object)
{
    // Silently exit on bad removes
    if (object == NULL || !set_.contains(object))
        return;

    // "contents" signals require a QSet as a parameter
    QSet<DataObject*> to_remove;
    to_remove.insert(object);

    notify_contents_to_be_altered(NULL, &to_remove);

    bool removed = set_.remove(object);
    Q_ASSERT(removed);
    removed = list_.removeOne(object);
    Q_ASSERT(removed);
    Q_UNUSED(removed);

    notify_contents_altered(NULL, &to_remove);

    Sanity();
}

/*!
 * \brief DataCollection::RemoveAt
 * \param index
 */
void DataCollection::RemoveAt(int index)
{
    if (index >= 0 && index < Count())
        Remove(GetAt(index));
}

/*!
 * \brief DataCollection::RemoveMany
 * \param objects
 */
void DataCollection::RemoveMany(const QSet<DataObject *> &objects)
{
    if (objects.count() == 0)
        return;

    // Want to only report DataObjects that are actually removed
    QSet<DataObject*> to_remove;
    DataObject* object;
    foreach (object, objects) {
        if (object != NULL && set_.contains(object))
            to_remove.insert(object);
    }

    if (to_remove.count() == 0)
        return;

    notify_contents_to_be_altered(NULL, &to_remove);

    foreach (object, to_remove) {
        bool removed = list_.removeOne(object);
        Q_ASSERT(removed);
        Q_UNUSED(removed);
    }

    set_.subtract(to_remove);

    notify_contents_altered(NULL, &to_remove);

    Sanity();
}

/*!
 * \brief DataCollection::Clear
 */
void DataCollection::Clear()
{
    if (list_.count() == 0) {
        Q_ASSERT(set_.count() == 0);

        return;
    }

    // save copy for signals
    QSet<DataObject*> all(set_);

    notify_contents_to_be_altered(NULL, &all);

    list_.clear();
    set_.clear();

    notify_contents_altered(NULL, &all);

    Sanity();
}

/*!
 * \brief DataCollection::GetAll
 * \return
 */
const QList<DataObject*>& DataCollection::GetAll() const
{
    return list_;
}

/*!
 * \brief DataCollection::GetAsSet
 * \return
 */
const QSet<DataObject*>& DataCollection::GetAsSet() const
{
    return set_;
}

/*!
 * \brief DataCollection::Contains
 * \param object
 * \return
 */
bool DataCollection::Contains(DataObject* object) const
{
    return set_.contains(object);
}

/*!
 * \brief DataCollection::ContainsAll
 * \param collection
 * \return
 */
bool DataCollection::ContainsAll(DataCollection* collection) const
{
    return set_.contains(collection->set_);
}

DataObject* DataCollection::GetAt(int index) const
{
    return (index >= 0 && index < list_.size()) ? list_[index] : NULL;
}

/*!
 * \brief DataCollection::IndexOf
 * \param object
 * \return
 */
int DataCollection::IndexOf(DataObject* object) const
{
    if (!set_.contains(object))
        return -1;

    int index = list_.indexOf(object);

    // Testing with set_ should prevent this possibility
    Q_ASSERT(index >= 0);

    return index;
}

/*!
 * \brief DataCollection::SetComparator
 * \param comparator
 */
void DataCollection::SetComparator(DataObjectComparator comparator)
{
    if (comparator_ == comparator)
        return;

    comparator_ = (comparator != NULL) ? comparator : DefaultDataObjectComparator;

    Resort(true);
}

/*!
 * \brief DataCollection::comparator
 * \return
 */
DataObjectComparator DataCollection::comparator() const
{
    return comparator_;
}

/*!
 * \brief DataCollection::DefaultDataObjectComparator
 * Default comparator uses DataObjectNumber
 * NOTE: this comparator function expects the API contract of
 *       DataObject::number() to return the same value for the same logical
 *       data object across invocations of Gallery. Right now, this contract
 *       is tenuously maintained. See the TODO item in DataObject.h.
 * \param a
 * \param b
 * \return
 */
bool DataCollection::DefaultDataObjectComparator(DataObject* a, DataObject* b)
{
    return a->number() < b->number();
}

/*!
 * \brief DataCollection::Sanity
 */
void DataCollection::Sanity() const
{
    Q_ASSERT(list_.count() == set_.count());
}

/*!
 * \brief DataCollection::BinaryListInsert
 * \param object
 */
void DataCollection::BinaryListInsert(DataObject* object)
{
    int index = -1;

    int low = 0;
    int high = list_.count();
    for (;;) {
        if (low == high) {
            index = low;

            break;
        }

        int mid = low + ((high - low) / 2);
        DataObject* midObject = list_[mid];

        if (comparator_(object, midObject)) {
            // lowerThan
            high = mid;
        } else if (comparator_(midObject, object)) {
            // higherThan
            low = mid + 1;
        } else {
            // equalTo
            index = mid;

            break;
        }
    }

    Q_ASSERT(index >= 0 && index <= list_.count());

    list_.insert(index, object);
}

/*!
 * \brief DataCollection::Resort
 * \param fire_signal
 */
void DataCollection::Resort(bool fire_signal)
{
    if (Count() <= 1)
        return;

    qSort(list_.begin(), list_.end(), comparator_);

    if (fire_signal)
        notify_ordering_altered();
}

/*!
 * \brief DataCollection::SetInternalName
 * \param name
 */
void DataCollection::SetInternalName(const QString& name)
{
    name_ = name.toUtf8();
}

/*!
 * \brief DataCollection::ToString
 * \return
 */
const char* DataCollection::ToString() const
{
    return name_.data();
}

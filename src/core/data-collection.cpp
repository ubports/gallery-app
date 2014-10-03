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

#include "data-collection.h"
#include "data-object.h"

#include <QQmlEngine>

/*!
 * \brief DataCollection::DataCollection
 * \param name
 */
DataCollection::DataCollection(const QString& name)
    : m_name(name.toUtf8()), m_comparator(defaultDataObjectComparator)
{
    // All DataCollections are registered as C++ ownership; QML should never GC them
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/*!
 * \brief DataCollection::notifyContentsToBeChanged
 * \param added
 * \param removed
 */
void DataCollection::notifyContentsToBeChanged(const QSet<DataObject*>* added,
                                               const QSet<DataObject*>* removed)
{
    emit contentsAboutToBeChanged(added, removed);
}

/*!
 * \brief DataCollection::notifyContentsChanged
 * \param added
 * \param removed
 */
void DataCollection::notifyContentsChanged(const QSet<DataObject*>* added,
                                           const QSet<DataObject*>* removed,
                                           bool notify)
{
    emit contentsChanged(added, removed, notify);
}

/*!
 * \brief DataCollection::notifyOrderingChanged
 */
void DataCollection::notifyOrderingChanged()
{
    emit orderingChanged();
}

/*!
 * \brief DataCollection::count
 * \return
 */
int DataCollection::count() const
{
    return m_list.size();
}

/*!
 * \brief DataCollection::add
 * \param object
 */
void DataCollection::add(DataObject* object)
{
    Q_ASSERT(object != NULL);

    // Silently prevent double-adds
    if (m_set.contains(object))
        return;

    // The "contents" signals require a QSet as a parameter
    QSet<DataObject*> to_add;
    to_add.insert(object);

    notifyContentsToBeChanged(&to_add, NULL);

    // Cheaper to binary insert single item than append it to list and do a
    // complete re-sort
    binaryListInsert(object);
    m_set.insert(object);

    notifyContentsChanged(&to_add, NULL, true);

    sanity();
}

/*!
 * \brief DataCollection::addMany
 * \param objects
 */
void DataCollection::addMany(const QSet<DataObject*>& objects)
{
    if (objects.count() == 0)
        return;

    // Silently prevent double-adds (as well as create a QList for a single
    // append operation on the internal list)
    QList<DataObject*> to_add_list;
    DataObject* object;
    foreach (object, objects) {
        Q_ASSERT(object != NULL);

        if (!m_set.contains(object))
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

    notifyContentsToBeChanged(&to_add, NULL);

    // use binary insertion if only adding one object
    if (add_count == 1) {
        object = to_add_list[0];

        binaryListInsert(object);
        m_set.insert(object);
    } else {
        m_list.append(to_add_list);
        m_set.unite(to_add);

        resort(false);
    }

    notifyContentsChanged(&to_add, NULL, true);

    sanity();
}

/*!
 * \brief DataCollection::remove
 * \param object
 */
void DataCollection::remove(DataObject* object, bool notify)
{
    // Silently exit on bad removes
    if (object == NULL || !m_set.contains(object))
        return;

    // "contents" signals require a QSet as a parameter
    QSet<DataObject*> to_remove;
    to_remove.insert(object);

    notifyContentsToBeChanged(NULL, &to_remove);

    bool removed = m_set.remove(object);
    Q_ASSERT(removed);
    removed = m_list.removeOne(object);
    Q_ASSERT(removed);
    Q_UNUSED(removed);

    notifyContentsChanged(NULL, &to_remove, notify);

    sanity();
}

/*!
 * \brief DataCollection::removeAt
 * \param index
 */
void DataCollection::removeAt(int index)
{
    if (index >= 0 && index < count())
        remove(getAt(index), true);
}

/*!
 * \brief DataCollection::removeMany
 * \param objects
 */
void DataCollection::removeMany(const QSet<DataObject *> &objects, bool notify)
{
    if (objects.count() == 0)
        return;

    // Want to only report DataObjects that are actually removed
    QSet<DataObject*> to_remove;
    DataObject* object;
    foreach (object, objects) {
        if (object != NULL && m_set.contains(object))
            to_remove.insert(object);
    }

    if (to_remove.count() == 0)
        return;

    notifyContentsToBeChanged(NULL, &to_remove);

    foreach (object, to_remove) {
        bool removed = m_list.removeOne(object);
        Q_ASSERT(removed);
        Q_UNUSED(removed);
    }

    m_set.subtract(to_remove);

    notifyContentsChanged(NULL, &to_remove, notify);

    sanity();
}

/*!
 * \brief DataCollection::clear
 */
void DataCollection::clear()
{
    if (m_list.count() == 0) {
        Q_ASSERT(m_set.count() == 0);

        return;
    }

    // save copy for signals
    QSet<DataObject*> all(m_set);

    notifyContentsToBeChanged(NULL, &all);

    m_list.clear();
    m_set.clear();

    notifyContentsChanged(NULL, &all, true);

    sanity();
}

/*!
 * \brief DataCollection::getAll
 * \return
 */
const QList<DataObject*>& DataCollection::getAll() const
{
    return m_list;
}

/*!
 * \brief DataCollection::getAsSet
 * \return
 */
const QSet<DataObject*>& DataCollection::getAsSet() const
{
    return m_set;
}

/*!
 * \brief DataCollection::contains
 * \param object
 * \return
 */
bool DataCollection::contains(DataObject* object) const
{
    return m_set.contains(object);
}

/*!
 * \brief DataCollection::containsAll
 * \param collection
 * \return
 */
bool DataCollection::containsAll(DataCollection* collection) const
{
    return m_set.contains(collection->m_set);
}

/*!
 * \brief DataCollection::getAt
 * \param index
 * \return
 */
DataObject* DataCollection::getAt(int index) const
{
    return (index >= 0 && index < m_list.size()) ? m_list[index] : NULL;
}

/*!
 * \brief DataCollection::indexOf
 * \param object
 * \return
 */
int DataCollection::indexOf(DataObject* object) const
{
    if (!m_set.contains(object))
        return -1;

    int index = m_list.indexOf(object);

    // Testing with set_ should prevent this possibility
    Q_ASSERT(index >= 0);

    return index;
}

/*!
 * \brief DataCollection::setComparator
 * \param comparator
 */
void DataCollection::setComparator(DataObjectComparator comparator)
{
    if (m_comparator == comparator)
        return;

    m_comparator = (comparator != NULL) ? comparator : defaultDataObjectComparator;

    resort(true);
}

/*!
 * \brief DataCollection::comparator
 * \return
 */
DataObjectComparator DataCollection::comparator() const
{
    return m_comparator;
}

/*!
 * \brief DataCollection::defaultDataObjectComparator
 * Default comparator uses DataObjectNumber
 * NOTE: this comparator function expects the API contract of
 *       DataObject::number() to return the same value for the same logical
 *       data object across invocations of Gallery. Right now, this contract
 *       is tenuously maintained. See the TODO item in DataObject.h.
 * \param a
 * \param b
 * \return
 */
bool DataCollection::defaultDataObjectComparator(DataObject* a, DataObject* b)
{
    return a->number() < b->number();
}

/*!
 * \brief DataCollection::const
 */
void DataCollection::sanity() const
{
    Q_ASSERT(m_list.count() == m_set.count());
}

/*!
 * \brief DataCollection::binaryListInsert
 * \param object
 */
void DataCollection::binaryListInsert(DataObject* object)
{
    int index = -1;

    int low = 0;
    int high = m_list.count();
    for (;;) {
        if (low == high) {
            index = low;

            break;
        }

        int mid = low + ((high - low) / 2);
        DataObject* midObject = m_list[mid];

        if (m_comparator(object, midObject)) {
            // lowerThan
            high = mid;
        } else if (m_comparator(midObject, object)) {
            // higherThan
            low = mid + 1;
        } else {
            // equalTo
            index = mid;

            break;
        }
    }

    Q_ASSERT(index >= 0 && index <= m_list.count());

    m_list.insert(index, object);
}

/*!
 * \brief DataCollection::resort
 * \param fire_signal
 */
void DataCollection::resort(bool fire_signal)
{
    if (count() <= 1)
        return;

    qSort(m_list.begin(), m_list.end(), m_comparator);

    if (fire_signal)
        notifyOrderingChanged();
}

/*!
 * \brief DataCollection::setInternalName
 * \param name
 */
void DataCollection::setInternalName(const QString& name)
{
    m_name = name.toUtf8();
}

/*!
 * \brief DataCollection::toString
 * \return
 */
const char* DataCollection::toString() const
{
    return m_name.data();
}

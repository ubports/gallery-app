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

#include "container-source.h"

/*!
 * \brief ContainerSource::ContainerSource
 * \param parent
 * \param name
 * \param comparator
 */
ContainerSource::ContainerSource(QObject * parent, const QString& name, DataObjectComparator comparator)
    : DataSource(parent), m_contained(QString("Container for ") + QString(name))
{
    m_contained.setComparator(comparator);

    QObject::connect(&m_contained,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)),
                     this,
                     SLOT(onContentsChanged(const QSet<DataObject*>*, const QSet<DataObject*>*, bool)));
}

/*!
 * \brief ContainerSource::attach
 * \param object
 */
void ContainerSource::attach(DataObject* object)
{
    m_contained.add(object);
}

/*!
 * \brief ContainerSource::attachMany
 * \param objects
 */
void ContainerSource::attachMany(const QSet<DataObject*>& objects)
{
    m_contained.addMany(objects);
}

/*!
 * \brief ContainerSource::detach
 * \param object
 */
void ContainerSource::detach(DataObject* object, bool notify)
{
    m_contained.remove(object, notify);
}

/*!
 * \brief ContainerSource::detachMany
 * \param objects
 */
void ContainerSource::detachMany(const QSet<DataObject*>& objects)
{
    m_contained.removeMany(objects, true);
}

/*!
 * \brief ContainerSource::contains
 * \param object
 * \return
 */
bool ContainerSource::contains(DataObject* object) const
{
    return m_contained.contains(object);
}

/*!
 * \brief ContainerSource::containsAll
 * \param collection
 * \return
 */
bool ContainerSource::containsAll(ContainerSource* collection) const
{
    return m_contained.containsAll(&collection->m_contained);
}

/*!
 * \brief ContainerSource::containedCount
 * \return
 */
int ContainerSource::containedCount() const
{
    return m_contained.count();
}

/*!
 * \brief ContainerSource::contained
 * \return
 */
const ViewCollection* ContainerSource::contained() const
{
    return &m_contained;
}

/*!
 * \brief ContainerSource::notifyContainerContentsChanged
 * \param added
 * \param removed
 */
void ContainerSource::notifyContainerContentsChanged(const QSet<DataObject*>* added,
                                                     const QSet<DataObject*>* removed)
{
    emit containerContentsChanged(added, removed);
}

/*!
 * \brief ContainerSource::onContentsChanged
 * \param added
 * \param removed
 */
void ContainerSource::onContentsChanged(const QSet<DataObject*>* added,
                                        const QSet<DataObject*>* removed,
                                        bool notify)
{
    notifyContainerContentsChanged(added, removed);
}

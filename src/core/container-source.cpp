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
    : DataSource(parent), contained_(QString("Container for ") + QString(name))
{
    contained_.SetComparator(comparator);

    QObject::connect(&contained_,
                     SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
                     this,
                     SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
}

/*!
 * \brief ContainerSource::Attach
 * \param object
 */
void ContainerSource::Attach(DataObject* object)
{
    contained_.Add(object);
}

/*!
 * \brief ContainerSource::AttachMany
 * \param objects
 */
void ContainerSource::AttachMany(const QSet<DataObject*>& objects)
{
    contained_.AddMany(objects);
}

/*!
 * \brief ContainerSource::Detach
 * \param object
 */
void ContainerSource::Detach(DataObject* object)
{
    contained_.Remove(object);
}

/*!
 * \brief ContainerSource::DetachMany
 * \param objects
 */
void ContainerSource::DetachMany(const QSet<DataObject*>& objects)
{
    contained_.RemoveMany(objects);
}

/*!
 * \brief ContainerSource::Contains
 * \param object
 * \return
 */
bool ContainerSource::Contains(DataObject* object) const
{
    return contained_.Contains(object);
}

/*!
 * \brief ContainerSource::ContainsAll
 * \param collection
 * \return
 */
bool ContainerSource::ContainsAll(ContainerSource* collection) const
{
    return contained_.ContainsAll(&collection->contained_);
}

/*!
 * \brief ContainerSource::ContainedCount
 * \return
 */
int ContainerSource::ContainedCount() const
{
    return contained_.Count();
}

/*!
 * \brief ContainerSource::contained
 * \return
 */
const ViewCollection* ContainerSource::contained() const
{
    return &contained_;
}

/*!
 * \brief ContainerSource::notify_container_contents_altered
 * \param added
 * \param removed
 */
void ContainerSource::notify_container_contents_altered(const QSet<DataObject*>* added,
                                                        const QSet<DataObject*>* removed)
{
    emit container_contents_altered(added, removed);
}

/*!
 * \brief ContainerSource::on_contents_altered
 * \param added
 * \param removed
 */
void ContainerSource::on_contents_altered(const QSet<DataObject*>* added,
                                          const QSet<DataObject*>* removed)
{
    notify_container_contents_altered(added, removed);
}

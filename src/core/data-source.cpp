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

#include "data-source.h"
#include "source-collection.h"

/*!
 * \brief DataSource::DataSource
 * \param parent
 */
DataSource::DataSource(QObject * parent)
    : DataObject(parent), membership_(NULL)
{
}

/*!
 * \brief DataSource::Destroy
 * This is only called by SourceCollection
 * NOTE: Do not assert or check (or rely on) membership_ to be set; Destroy
 * operation requires that objects be removed from SourceCollection prior
 * to destruction; merely have to rely on contract to ensure that this
 * method is not called on an orphaned DataSource
 * \param destroy_backing
 */
void DataSource::Destroy(bool destroy_backing)
{
    // notify subscribers of imminent doom, destroy, then notify of carnage wreaked
    notify_destroying(destroy_backing, false);
    DestroySource(destroy_backing, false);
    notify_destroyed(destroy_backing, false);
}

/*!
 * \brief DataSource::DestroyOrphan Used to destroy a DataSource that is not a member of a SourceCollection
 * Use SourceCollection.Destroy() / DestroyAll() for DataSources attached to one a SourceCollection
 * NOTE: Do not assert or check (or rely on) membership_ to be set; Destroy
 *   operation requires that objects be removed from SourceCollection prior
 *   to destruction; merely have to rely on contract to ensure that this
 *   method is not called on an attached DataSource
 * \param destroy_backing
 */
void DataSource::DestroyOrphan(bool destroy_backing)
{
    // like Destroy(), notify before and after destruction
    notify_destroying(destroy_backing, true);
    DestroySource(destroy_backing, true);
    notify_destroyed(destroy_backing, true);
}

/*!
 * \brief DataSource::notify_destroying
 * \param destroying_backing
 * \param as_orphan
 */
void DataSource::notify_destroying(bool destroying_backing, bool as_orphan)
{
    emit destroying(destroying_backing, as_orphan);
}

/*!
 * \brief DataSource::notify_destroyed
 * \param destroyed_backing
 * \param as_orphan
 */
void DataSource::notify_destroyed(bool destroyed_backing, bool as_orphan)
{
    emit destroyed(destroyed_backing, as_orphan);
}

/*!
 * \brief DataSource::member_of
 * \return returns NULL if not a member of any SourceCollection (i.e. is orphaned).
 */
SourceCollection* DataSource::member_of() const
{
    return membership_;
}

/*!
 * \brief DataSource::set_membership
 * Set to NULL if no longer a member of a DataCollection.  Will assert if
 * membership to a non-null DataCollection is set while already a member of
 * another collection.
 * \param collection
 */
void DataSource::set_membership(SourceCollection* collection)
{
    if (collection != NULL) {
        Q_ASSERT(membership_ == NULL);
    }

    membership_ = collection;
}

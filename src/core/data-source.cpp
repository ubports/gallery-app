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
    : DataObject(parent), m_membership(NULL)
{
}

/*!
 * \brief DataSource::destroy
 * This is only called by SourceCollection
 * NOTE: Do not assert or check (or rely on) membership_ to be set; Destroy
 * operation requires that objects be removed from SourceCollection prior
 * to destruction; merely have to rely on contract to ensure that this
 * method is not called on an orphaned DataSource
 * \param destroy_backing
 */
void DataSource::destroy(bool destroy_backing)
{
    // notify subscribers of imminent doom, destroy, then notify of carnage wreaked
    notifyDestroying(destroy_backing, false);
    destroySource(destroy_backing, false);
    notifyDestroyed(destroy_backing, false);
}

/*!
 * \brief DataSource::destroyOrphan Used to destroy a DataSource that is not a member of a SourceCollection
 * Use SourceCollection.Destroy() / DestroyAll() for DataSources attached to one a SourceCollection
 * NOTE: Do not assert or check (or rely on) membership_ to be set; Destroy
 *   operation requires that objects be removed from SourceCollection prior
 *   to destruction; merely have to rely on contract to ensure that this
 *   method is not called on an attached DataSource
 * \param destroy_backing
 */
void DataSource::destroyOrphan(bool destroy_backing)
{
    // like Destroy(), notify before and after destruction
    notifyDestroying(destroy_backing, true);
    destroySource(destroy_backing, true);
    notifyDestroyed(destroy_backing, true);
}

/*!
 * \brief DataSource::notifyDestroying
 * \param destroying_backing
 * \param as_orphan
 */
void DataSource::notifyDestroying(bool destroying_backing, bool as_orphan)
{
    emit destroying(destroying_backing, as_orphan);
}

/*!
 * \brief DataSource::notifyDestroyed
 * \param destroyed_backing
 * \param as_orphan
 */
void DataSource::notifyDestroyed(bool destroyed_backing, bool as_orphan)
{
    emit destroyed(destroyed_backing, as_orphan);
}

/*!
 * \brief DataSource::memberOf
 * \return returns NULL if not a member of any SourceCollection (i.e. is orphaned).
 */
SourceCollection* DataSource::memberOf() const
{
    return m_membership;
}

/*!
 * \brief DataSource::setMembership
 * Set to NULL if no longer a member of a DataCollection.  Will assert if
 * membership to a non-null DataCollection is set while already a member of
 * another collection.
 * \param collection
 */
void DataSource::setMembership(SourceCollection* collection)
{
    if (collection != NULL) {
        Q_ASSERT(m_membership == NULL);
    }

    m_membership = collection;
}

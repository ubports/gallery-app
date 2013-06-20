/*
 * Copyright (C) 2012 Canonical Ltd
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

#include "event.h"

// media
#include "media-collection.h"

#include <QString>

/*!
 * \brief Event::Event
 * \param parent
 */
Event::Event(QObject* parent)
    : ContainerSource(parent, "Event (undated)", MediaCollection::exposureDateTimeDescendingComparator)
{
}

/*!
 * \brief Event::Event
 * \param parent
 * \param date
 */
Event::Event(QObject* parent, const QDate& date)
    : ContainerSource(parent, QString("Event for ") + date.toString(),
                      MediaCollection::exposureDateTimeDescendingComparator), m_date(date)
{
}

/*!
 * \brief Event::date
 * \return
 */
const QDate& Event::date() const
{
    return m_date;
}

/*!
 * \brief Event::startDateTime
 * \return
 */
QDateTime Event::startDateTime() const
{
    return QDateTime(date());
}

/*!
 * \brief Event::endDateTime
 * \return
 */
QDateTime Event::endDateTime() const
{
    return QDateTime(date(), QTime(23, 59, 59, 999));
}

/*!
 * \brief Event::DestroySource \reimp
 * \param destroyBacking
 * \param asOrphan
 */
void Event::destroySource(bool destroyBacking, bool asOrphan)
{
    // Event is a virtual DataSource generated as a result of MediaSources added
    // and removed from the system, so nothing to destroy
}

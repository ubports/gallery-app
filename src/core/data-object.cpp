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

#include "data-object.h"

#include <QQmlEngine>

DataObjectNumber DataObject::m_nextNumber = 0;

/*!
 * \brief DataObject::DataObject
 * \param parent
 */
DataObject::DataObject(QObject * parent)
    : QObject(parent), m_number(m_nextNumber++)
{
    // All DataObjects are registered as C++ ownership; QML should never GC them
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/*!
 * \brief DataObject::number
 * TODO: number() should return the same value for the same DataObject across
 *       invocations of Gallery. Right now, this API contract is maintained
 *       implicitly and in a particularly fragile way. We should fix this.
 * \return
 */
DataObjectNumber DataObject::number() const
{
    return m_number;
}

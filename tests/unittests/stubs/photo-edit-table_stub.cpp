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
 * Charles Lindsay <chaz@yorba.org>
 */

#include "photo-edit-table.h"
#include "database.h"

// photo
#include "photo-edit-state.h"

#include <QStringList>
#include <QVariant>

PhotoEditTable::PhotoEditTable(Database* db, QObject* parent)
    : QObject(parent),
      m_db(db)
{
}

PhotoEditState PhotoEditTable::editState(qint64 mediaId) const
{
    Q_UNUSED(mediaId);
    PhotoEditState edit_state;
    return edit_state;
}

void PhotoEditTable::setEditState(qint64 mediaId,
                                  const PhotoEditState& editState)
{
    Q_UNUSED(mediaId);
    Q_UNUSED(editState);
}

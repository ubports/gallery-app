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
 * Eric Gregory <eric@yorba.org>
 * Charles Lindsay <chaz@yorba.org>
 */

#include "database.h"
#include "album-table.h"
#include "media-table.h"

#include <QFile>

Database::Database(const QString &databaseDir, const QString &schemaDirectory,
                   QObject* parent) :
    QObject(parent),
    m_databaseDirectory(databaseDir),
    m_sqlSchemaDirectory(schemaDirectory),
    m_db(0)
{
    m_albumTable = new AlbumTable(this, this);
    m_mediaTable = new MediaTable(this, this);
}

Database::~Database()
{
    delete m_albumTable;
    delete m_mediaTable;
}

AlbumTable* Database::getAlbumTable() const
{
    return m_albumTable;
}

MediaTable* Database::getMediaTable() const
{
    return m_mediaTable;
}

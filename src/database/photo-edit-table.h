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

#ifndef PHOTOEDITTABLE_H
#define PHOTOEDITTABLE_H

#include <QObject>

class Database;
class PhotoEditState;

/*!
 * \brief The PhotoEditTable class
 */
class PhotoEditTable : public QObject
{
    Q_OBJECT

public:
    explicit PhotoEditTable(Database* db, QObject *parent = 0);

    PhotoEditState editState(qint64 mediaId) const;
    void setEditState(qint64 mediaId, const PhotoEditState& editState);

private:
    void prepareRow(qint64 mediaId);

    Database* m_db;
};

#endif // PHOTOEDITTABLE_H

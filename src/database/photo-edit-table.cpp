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

#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

/*!
 * \brief PhotoEditTable::PhotoEditTable
 * \param db
 * \param parent
 */
PhotoEditTable::PhotoEditTable(Database* db, QObject* parent)
    : QObject(parent), m_db(db)
{
}

/*!
 * \brief PhotoEditTable::editState
 * \param mediaId
 * \return
 */
PhotoEditState PhotoEditTable::editState(qint64 mediaId) const
{
    PhotoEditState edit_state;

    QSqlQuery query(*m_db->getDB());
    query.prepare("SELECT crop_rectangle, is_enhanced, orientation "
                  "FROM PhotoEditTable "
                  "WHERE media_id = :media_id");

    query.bindValue(":media_id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);

    if (query.next()) {
        QStringList parts = query.value(0).toString().split(',');
        if (parts.count() == 4) {
            int x = parts[0].toInt();
            int y = parts[1].toInt();
            int width = parts[2].toInt();
            int height = parts[3].toInt();

            edit_state.crop_rectangle_ = QRect(x, y, width, height);
        }

        edit_state.is_enhanced_ = query.value(1).toBool();
        edit_state.orientation_ = static_cast<Orientation>(query.value(2).toInt());
    }

    return edit_state;
}

/*!
 * \brief PhotoEditTable::setEditState
 * \param mediaId
 * \param editState
 */
void PhotoEditTable::setEditState(qint64 mediaId,
                                  const PhotoEditState& editState)
{
    if (mediaId == INVALID_ID)
        return;

    prepareRow(mediaId);

    QString crop_rect_string = QString("%1,%2,%3,%4")
            .arg(editState.crop_rectangle_.x())
            .arg(editState.crop_rectangle_.y())
            .arg(editState.crop_rectangle_.width())
            .arg(editState.crop_rectangle_.height());

    QSqlQuery query(*m_db->getDB());

    query.prepare("UPDATE PhotoEditTable "
                  "SET crop_rectangle = :crop_rect_string, "
                  "is_enhanced = :is_enhanced, "
                  "orientation = :orientation "
                  "WHERE media_id = :media_id");
    query.bindValue(":media_id", mediaId);
    query.bindValue(":crop_rect_string", crop_rect_string);
    query.bindValue(":is_enhanced", editState.is_enhanced_);
    query.bindValue(":orientation", static_cast<int>(editState.orientation_));

    if (!query.exec())
        m_db->logSqlError(query);
}

/*!
 * \brief PhotoEditTable::prepareRow
 * \param mediaId
 */
void PhotoEditTable::prepareRow(qint64 mediaId)
{
    QSqlQuery query(*m_db->getDB());
    query.prepare("INSERT OR IGNORE INTO PhotoEditTable "
                  "(media_id) VALUES (:media_id)");

    query.bindValue(":media_id", mediaId);
    if (!query.exec())
        m_db->logSqlError(query);
}

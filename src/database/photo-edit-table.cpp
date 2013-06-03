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

#include <QSqlQuery>
#include <QVariant>

#include "photo-edit-table.h"
#include "database.h"
#include "photo/photo-edit-state.h"

/*!
 * \brief PhotoEditTable::PhotoEditTable
 * \param db
 * \param parent
 */
PhotoEditTable::PhotoEditTable(Database* db, QObject* parent)
    : QObject(parent), db_(db)
{
}

/*!
 * \brief PhotoEditTable::get_edit_state
 * \param media_id
 * \return
 */
PhotoEditState PhotoEditTable::get_edit_state(qint64 media_id) const
{
    PhotoEditState edit_state;

    QSqlQuery query(*db_->get_db());
    query.prepare("SELECT crop_rectangle, is_enhanced, orientation "
                  "FROM PhotoEditTable "
                  "WHERE media_id = :media_id");

    query.bindValue(":media_id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);

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
 * \brief PhotoEditTable::set_edit_state
 * \param media_id
 * \param edit_state
 */
void PhotoEditTable::set_edit_state(qint64 media_id,
                                    const PhotoEditState& edit_state)
{
    if (media_id == INVALID_ID)
        return;

    prepare_row(media_id);

    QString crop_rect_string = QString("%1,%2,%3,%4")
            .arg(edit_state.crop_rectangle_.x())
            .arg(edit_state.crop_rectangle_.y())
            .arg(edit_state.crop_rectangle_.width())
            .arg(edit_state.crop_rectangle_.height());

    QSqlQuery query(*db_->get_db());

    query.prepare("UPDATE PhotoEditTable "
                  "SET crop_rectangle = :crop_rect_string, "
                  "is_enhanced = :is_enhanced, "
                  "orientation = :orientation "
                  "WHERE media_id = :media_id");
    query.bindValue(":media_id", media_id);
    query.bindValue(":crop_rect_string", crop_rect_string);
    query.bindValue(":is_enhanced", edit_state.is_enhanced_);
    query.bindValue(":orientation", static_cast<int>(edit_state.orientation_));

    if (!query.exec())
        db_->log_sql_error(query);
}

/*!
 * \brief PhotoEditTable::prepare_row
 * \param media_id
 */
void PhotoEditTable::prepare_row(qint64 media_id)
{
    QSqlQuery query(*db_->get_db());
    query.prepare("INSERT OR IGNORE INTO PhotoEditTable "
                  "(media_id) VALUES (:media_id)");

    query.bindValue(":media_id", media_id);
    if (!query.exec())
        db_->log_sql_error(query);
}

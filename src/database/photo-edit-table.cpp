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

#include "photo-edit-table.h"

PhotoEditTable::PhotoEditTable(Database* db, QObject* parent)
    : QObject(parent), db_(db) {
}

QRect PhotoEditTable::get_crop_rectangle(qint64 media_id) const {
  QRect crop_rect;

  QSqlQuery query(*db_->get_db());
  query.prepare("SELECT crop_rectangle FROM PhotoEditTable "
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

      crop_rect = QRect(x, y, width, height);
    }
  }

  return crop_rect;
}

void PhotoEditTable::set_crop_rectangle(qint64 media_id,
    const QRect &crop_rect) {
  QString crop_rect_string = QString("%1,%2,%3,%4")
      .arg(crop_rect.x())
      .arg(crop_rect.y())
      .arg(crop_rect.width())
      .arg(crop_rect.height());

  // TODO: this might not work well once other edit operations need to get
  // stored here.  For example, if there's already a row with a value for
  // autoenhance, I believe this would delete that value first before setting
  // the value for the crop rectangle.
  QSqlQuery query(*db_->get_db());
  query.prepare("INSERT OR REPLACE INTO PhotoEditTable "
                "(media_id, crop_rectangle) "
                "VALUES (:media_id, :crop_rectangle)");

  query.bindValue(":media_id", media_id);
  query.bindValue(":crop_rectangle", crop_rect_string);
  if (!query.exec())
    db_->log_sql_error(query);
}

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

class PhotoEditTable : public QObject {
  Q_OBJECT

 public:
  explicit PhotoEditTable(Database* db, QObject *parent = 0);

  PhotoEditState get_edit_state(qint64 media_id) const;
  void set_edit_state(qint64 media_id, const PhotoEditState& edit_state);

 private:
  void prepare_row(qint64 media_id);

  Database* db_;
};

#endif // PHOTOEDITTABLE_H

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

#include "database.h"

class PhotoEditTable : public QObject {
  Q_OBJECT

 public:
  explicit PhotoEditTable(Database* db, QObject *parent = 0);

  QRect get_crop_rectangle(qint64 media_id) const;
  void set_crop_rectangle(qint64 media_id, const QRect& crop_rect);

  bool get_is_enhanced(qint64 media_id) const;
  void set_is_enhanced(qint64 media_id, bool is_enhanced);

 private:
  void prepare_row(qint64 media_id);

  Database* db_;
};

#endif // PHOTOEDITTABLE_H

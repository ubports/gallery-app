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

#ifndef GALLERY_QML_EVENT_MARKER_H_
#define GALLERY_QML_EVENT_MARKER_H_

#include <QObject>
#include <QDate>
#include <QDateTime>
#include <QVariant>
#include <QtDeclarative>

#include "core/data-source.h"

class QmlEventMarker : public DataSource {
  Q_OBJECT
  Q_PROPERTY(QDate date READ date NOTIFY date_changed)
  Q_PROPERTY(QDateTime startDateTime READ start_date_time NOTIFY date_changed)
  Q_PROPERTY(QDateTime endDateTime READ end_date_time NOTIFY date_changed)
  
 signals:
  void date_changed();
  
 public:
  QmlEventMarker();
  explicit QmlEventMarker(const QDate& date);
  
  static void RegisterType();
  
  QDate date() const;
  QDateTime start_date_time() const;
  QDateTime end_date_time() const;
  
 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
 private:
  QDate date_;
};

QML_DECLARE_TYPE(QmlEventMarker);

#endif  // GALLERY_QML_EVENT_MARKER_H_

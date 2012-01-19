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

#include "qml/qml-event-marker.h"

QmlEventMarker::QmlEventMarker()
  : DataSource("QmlEventMarker") {
}

QmlEventMarker::QmlEventMarker(const QDate& date)
  : DataSource("QmlEventMarker"), date_(date) {
}

void QmlEventMarker::RegisterType() {
  qmlRegisterType<QmlEventMarker>("Gallery", 1, 0, "EventMarker");
}

QDate QmlEventMarker::date() const {
  return date_;
}

QString QmlEventMarker::pretty_date() const {
  return date_.toString("d-M-yyyy");
}

QDateTime QmlEventMarker::date_time() const {
  return QDateTime(date_);
}

void QmlEventMarker::DestroySource(bool destroy_backing, bool as_orphan) {
}

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

#include "event/event.h"

#include <QString>

#include "util/collections.h"

Event::Event()
  : ContainerSource("Event (undated)") {
}

Event::Event(const QDate& date)
  : ContainerSource(QString("Event for ") + date.toString()), date_(date) {
}

void Event::RegisterType() {
  qmlRegisterType<Event>("Gallery", 1, 0, "Event");
}

const QDate& Event::date() const {
  return date_;
}

QDateTime Event::start_date_time() const {
  return QDateTime(date());
}

QDateTime Event::end_date_time() const {
  return QDateTime(date(), QTime(23, 59, 59, 999));
}

void Event::DestroySource(bool destroy_backing, bool as_orphan) {
  // Event is a virtual DataSource generated as a result of MediaSources added
  // and removed from the system, so nothing to destroy
}

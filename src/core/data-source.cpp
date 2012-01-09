/*
 * Copyright (C) 2011 Canonical Ltd
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

#include "core/data-source.h"

DataSource::DataSource(const QString& name)
  : DataObject(name), membership_(NULL) {
}

void DataSource::Destroy(bool destroy_backing) {
  // first notify subscribers of imminent doom
  notify_destroying(destroy_backing);
  
  // do it
  DestroySource(destroy_backing);
  
  // notify subscribers of carnage
  notify_destroyed(destroy_backing);
}

void DataSource::notify_destroying(bool destroying_backing) {
  emit destroying(destroying_backing);
}

void DataSource::notify_destroyed(bool destroyed_backing) {
  emit destroyed(destroyed_backing);
}

SourceCollection* DataSource::member_of() const {
  return membership_;
}

void DataSource::set_membership(SourceCollection* collection) {
  if (collection != NULL) {
    Q_ASSERT(membership_ == NULL);
  }
  
  membership_ = collection;
}

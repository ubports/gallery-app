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

#include "core/container-source.h"

ContainerSource::ContainerSource(const QString& name, DataObjectComparator comparator)
  : DataSource(name), contained_(QString("Container for ") + QString(name)) {
  contained_.SetComparator(comparator);
  
  QObject::connect(&contained_,
    SIGNAL(contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)),
    this,
    SLOT(on_contents_altered(const QSet<DataObject*>*, const QSet<DataObject*>*)));
}

void ContainerSource::Attach(DataObject* object) {
  contained_.Add(object);
}

void ContainerSource::AttachMany(const QSet<DataObject*>& objects) {
  contained_.AddMany(objects);
}

void ContainerSource::Detach(DataObject* object) {
  contained_.Remove(object);
}

void ContainerSource::DetachMany(const QSet<DataObject*>& objects) {
  contained_.RemoveMany(objects);
}

bool ContainerSource::Contains(DataObject* object) const {
  return contained_.Contains(object);
}

bool ContainerSource::ContainsAll(ContainerSource* collection) const {
  return contained_.ContainsAll(&collection->contained_);
}

int ContainerSource::ContainedCount() const {
  return contained_.Count();
}

const ViewCollection* ContainerSource::contained() const {
  return &contained_;
}

void ContainerSource::notify_container_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  emit container_contents_altered(added, removed);
}

void ContainerSource::on_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  notify_container_contents_altered(added, removed);
}

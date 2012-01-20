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

/**
  * A ContainerSource is a DataSource that holds other DataSources.  Generally
  * these are used for organizational features.
  */

#ifndef GALLERY_CONTAINER_SOURCE_H_
#define GALLERY_CONTAINER_SOURCE_H_

#include <QObject>
#include <QSet>

#include "core/data-object.h"
#include "core/data-source.h"
#include "core/view-collection.h"

class ContainerSource : public DataSource {
  Q_OBJECT
  
 signals:
  void container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 public:
  ContainerSource(const QString& name);
  
  void Attach(DataObject* object);
  void AttachMany(const QSet<DataObject*>& objects);
  
  bool Contains(DataObject* object) const;
  int ContainedCount() const;
  const ViewCollection* contained() const;
  
 protected:
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private slots:
  void on_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  ViewCollection contained_;
};

#endif  // GALLERY_CONTAINER_SOURCE_H_

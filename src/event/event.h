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

#ifndef GALLERY_EVENT_H_
#define GALLERY_EVENT_H_

#include <QObject>
#include <QDate>
#include <QDeclarativeListProperty>
#include <QList>
#include <QtDeclarative>

#include "core/container-source.h"
#include "media/media-source.h"

class Event : public ContainerSource {
  Q_OBJECT
  Q_PROPERTY(QDate date READ date NOTIFY date_altered)
  Q_PROPERTY(QDeclarativeListProperty<MediaSource> mediaSources READ qml_media_sources
    NOTIFY media_content_altered);
  
 signals:
  void date_altered();
  void media_content_altered();
  
 public:
  Event();
  explicit Event(const QDate &date);
  
  static void RegisterType();
  
  const QDate& date() const;
  
  QDeclarativeListProperty<MediaSource> qml_media_sources();
  
 protected:
  virtual void notify_container_contents_altered(const QSet<DataObject *> *added,
    const QSet<DataObject *> *removed);
  
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
 private:
  QDate date_;
  QList<MediaSource*> all_media_;
};

QML_DECLARE_TYPE(Event);

#endif  // GALLERY_EVENT_H_

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

#ifndef GALLERY_ALBUM_H_
#define GALLERY_ALBUM_H_

#include <QObject>
#include <QString>
#include <QList>

#include "container-source.h"
#include "media-source.h"

class Album : public ContainerSource {
  Q_OBJECT
  
 signals:
  void current_page_contents_altered();
  
 public:
  static const char *DEFAULT_NAME;
  
  Album();
  explicit Album(const QString &name);
  
  const QString& name() const;
  
  // Returns zero if album is closed
  int current_page() const;
  
  // TODO: Make a const method
  const QList<MediaSource*> &current_page_contents() const;
  
 protected:
  virtual void notify_current_page_contents_altered();
  
  virtual void DestroySource(bool destroy_backing);
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  QString name_;
  int current_page_;
  QList<MediaSource*> current_page_contents_;
};

#endif  // GALLERY_ALBUM_H_

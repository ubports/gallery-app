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

#include "album/album-page.h"
#include "album/album-template.h"
#include "core/container-source.h"
#include "core/source-collection.h"
#include "media/media-source.h"

class Album : public ContainerSource {
  Q_OBJECT
  
 signals:
  void current_page_contents_altered();
  
 public:
  static const char *DEFAULT_NAME;
  
  explicit Album(const AlbumTemplate& album_template);
  Album(const AlbumTemplate& album_template, const QString &name);
  
  const QString& name() const;
  const AlbumTemplate& album_template() const;
  
  // Returns a SourceCollection representing all AlbumPages held by this Album
  SourceCollection* pages();
  
  // Returns -1 if album is closed
  int current_page() const;
  
  bool IsClosed() const;
  
  int PageCount() const;
  
  // Returns NULL if page number is beyond bounds
  AlbumPage* GetPage(int page) const;
  
 protected:
  virtual void notify_current_page_contents_altered();
  
  virtual void DestroySource(bool destroy_backing);
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  const AlbumTemplate& album_template_;
  QString name_;
  int current_page_;
  SourceCollection pages_;
};

#endif  // GALLERY_ALBUM_H_

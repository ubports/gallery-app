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
#include <QDeclarativeListProperty>
#include <QString>
#include <QUrl>
#include <QtDeclarative>

#include "album/album-page.h"
#include "album/album-template.h"
#include "core/container-source.h"
#include "core/source-collection.h"
#include "media/media-source.h"

class Album : public ContainerSource {
  Q_OBJECT
  Q_PROPERTY(QString name READ name NOTIFY name_altered);
  Q_PROPERTY(int currentPage READ current_page WRITE set_current_page
    NOTIFY current_page_altered);
  Q_PROPERTY(QDeclarativeListProperty<AlbumPage> pages READ qml_pages
    NOTIFY pages_altered);
  Q_PROPERTY(QDeclarativeListProperty<MediaSource> allMediaSources
    READ qml_all_media_sources NOTIFY album_contents_altered);
  
 signals:
  void album_contents_altered();
  void current_page_contents_altered();
  void name_altered();
  void current_page_altered();
  void pages_altered();
  
 public:
  static const char *DEFAULT_NAME;
  
  Album();
  explicit Album(const AlbumTemplate& album_template);
  Album(const AlbumTemplate& album_template, const QString &name);
  virtual ~Album();
  
  static void RegisterType();
  
  Q_INVOKABLE void addMediaSource(QVariant vmedia);
  Q_INVOKABLE void addSelectedMediaSources(QVariant vmodel);
  
  const QString& name() const;
  const AlbumTemplate& album_template() const;
  bool is_closed() const;
  void close();
  int page_count() const;
  
  // Returns -1 if album is closed
  int current_page() const;
  
  // Pass -1 to close album
  void set_current_page(int page);
  
  // Returns a SourceCollection representing all AlbumPages held by this Album
  SourceCollection* pages();
  
  // Returns NULL if page number is beyond bounds
  AlbumPage* GetPage(int page) const;
  
  QDeclarativeListProperty<AlbumPage> qml_pages();
  QDeclarativeListProperty<MediaSource> qml_all_media_sources();
  
 protected:
  virtual void DestroySource(bool destroy_backing);
  
  virtual void notify_current_page_altered();
  
  virtual void notify_current_page_contents_altered();
  
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private slots:
  void on_album_page_content_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  const AlbumTemplate& album_template_;
  QString name_;
  int current_page_;
  SourceCollection* pages_;
  QList<MediaSource*> all_media_sources_;
  QList<AlbumPage*> all_album_pages_;
  
  void Init();
};

QML_DECLARE_TYPE(Album);

#endif  // GALLERY_ALBUM_H_

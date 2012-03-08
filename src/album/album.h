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
#include <QDateTime>
#include <QDeclarativeListProperty>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QtDeclarative>

#include "album/album-page.h"
#include "album/album-template.h"
#include "core/container-source.h"
#include "core/source-collection.h"
#include "media/media-source.h"

class Album : public ContainerSource {
  Q_OBJECT
  Q_PROPERTY(QString name READ name NOTIFY name_altered)
  Q_PROPERTY(QDateTime creationDateTime READ creation_date_time
    NOTIFY creation_date_time_altered)
  Q_PROPERTY(QVariant currentPage READ qml_current_page NOTIFY current_page_altered)
  Q_PROPERTY(int currentPageNumber READ current_page_number WRITE set_current_page_number
    NOTIFY current_page_number_altered)
  Q_PROPERTY(int pageCount READ page_count NOTIFY pages_altered)
  Q_PROPERTY(bool closed READ is_closed WRITE set_closed NOTIFY opened_closed)
  Q_PROPERTY(QDeclarativeListProperty<AlbumPage> pages READ qml_pages
    NOTIFY pages_altered)
  Q_PROPERTY(QDeclarativeListProperty<MediaSource> allMediaSources
    READ qml_all_media_sources NOTIFY album_contents_altered)
  
 signals:
  void album_contents_altered();
  void creation_date_time_altered();
  void current_page_altered();
  void current_page_number_altered();
  void current_page_contents_altered();
  void name_altered();
  void opened_closed();
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
  Q_INVOKABLE QVariant getPage(int page) const;
  
  const QString& name() const;
  const QDateTime& creation_date_time() const;
  const AlbumTemplate& album_template() const;
  bool is_closed() const;
  int page_count() const;
  
  int current_page_number() const;
  void set_current_page_number(int page);
  void set_closed(bool closed);
  
  // Returns a SourceCollection representing all AlbumPages held by this Album
  SourceCollection* pages();
  
  // Returns NULL if page number is beyond bounds
  AlbumPage* GetPage(int page) const;
  
  QVariant qml_current_page() const;
  QDeclarativeListProperty<AlbumPage> qml_pages();
  QDeclarativeListProperty<MediaSource> qml_all_media_sources();
  
 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
  virtual void notify_current_page_altered();

  virtual void notify_current_page_number_altered();
  
  virtual void notify_opened_closed();
  
  virtual void notify_current_page_contents_altered();
  
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private slots:
  void on_album_page_content_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  const AlbumTemplate& album_template_;
  QString name_;
  QDateTime creation_date_time_;
  int current_page_;
  bool closed_;
  SourceCollection* pages_;
  QList<MediaSource*> all_media_sources_;
  QList<AlbumPage*> all_album_pages_;
  bool refreshing_container_;
  
  void InitInstance();
};

QML_DECLARE_TYPE(Album)

#endif  // GALLERY_ALBUM_H_

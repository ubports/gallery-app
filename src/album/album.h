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
 * Charles Lindsay <chaz@yorba.org>
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

// Note about page numbers: the first page (number 0) is the front cover.  It
// always shows up on the right side of the spread.  The first "content" page
// is number 1, and it shows up on the left.  There are always an even number
// of content pages.  The last page (after the last content page) is the back
// cover, which always shows up on the left.  You can use
// first/lastContentPageNumber and total/contentPageCount to avoid off-by-one
// errors.
//
// Note about "current page number": the current page is the page on the left
// of the current spread.  This means that when the album is closed,
// currentPageNumber is actually -1 (since the cover on the right is page 0).
// You can use first/lastCurrentPageNumber to avoid off-by-one errors.
class Album : public ContainerSource {
  Q_OBJECT
  Q_PROPERTY(QString name READ name NOTIFY name_altered)
  Q_PROPERTY(QDateTime creationDateTime READ creation_date_time
    NOTIFY creation_date_time_altered)
  Q_PROPERTY(QDeclarativeListProperty<AlbumPage> contentPages READ qml_pages
    NOTIFY contentPagesAltered)
  Q_PROPERTY(QDeclarativeListProperty<MediaSource> allMediaSources
    READ qml_all_media_sources NOTIFY album_contents_altered)
  Q_PROPERTY(int firstContentPageNumber READ first_content_page_number NOTIFY contentPagesAltered)
  Q_PROPERTY(int lastContentPageNumber READ last_content_page_number NOTIFY contentPagesAltered)
  Q_PROPERTY(int totalPageCount READ total_page_count NOTIFY contentPagesAltered)
  Q_PROPERTY(int contentPageCount READ content_page_count NOTIFY contentPagesAltered)
  Q_PROPERTY(int firstCurrentPageNumber READ first_current_page_number NOTIFY contentPagesAltered)
  Q_PROPERTY(int lastCurrentPageNumber READ last_current_page_number NOTIFY contentPagesAltered)
  Q_PROPERTY(int currentPageNumber READ current_page_number WRITE set_current_page_number
    NOTIFY current_page_number_altered)
  Q_PROPERTY(bool closed READ is_closed WRITE set_closed NOTIFY opened_closed)
  
 signals:
  void album_contents_altered();
  void creation_date_time_altered();
  void current_page_number_altered();
  void current_page_contents_altered();
  void name_altered();
  void opened_closed();
  void contentPagesAltered();
  
 public:
  static const char *DEFAULT_NAME;
  static const int PAGES_PER_COVER;
  static const int FIRST_CURRENT_PAGE_NUMBER;

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
  int total_page_count() const;
  int content_page_count() const;
  int first_content_page_number() const;
  int last_content_page_number() const;
  int first_current_page_number() const;
  int last_current_page_number() const;
  int current_page_number() const;
  void set_current_page_number(int page);
  void set_closed(bool closed);
  
  // Returns a SourceCollection representing all AlbumPages held by this Album
  SourceCollection* content_pages();
  
  // Returns NULL if page number is beyond bounds
  AlbumPage* GetPage(int page) const;
  
  QDeclarativeListProperty<AlbumPage> qml_pages();
  QDeclarativeListProperty<MediaSource> qml_all_media_sources();
  
 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
  virtual void notify_current_page_number_altered();
  virtual void notify_opened_closed();
  virtual void notify_current_page_contents_altered();
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);

  int content_to_absolute_page_number(int content_page_number) const;
  int absolute_to_content_page_number(int absolute_page_number) const;
  
 private slots:
  void on_album_page_content_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  const AlbumTemplate& album_template_;
  QString name_;
  QDateTime creation_date_time_;
  int current_page_; // Page number of the left page of the current spread.
  bool closed_;
  SourceCollection* content_pages_;
  QList<MediaSource*> all_media_sources_;
  QList<AlbumPage*> all_album_pages_;
  bool refreshing_container_;
  
  void InitInstance();
};

QML_DECLARE_TYPE(Album)

#endif  // GALLERY_ALBUM_H_

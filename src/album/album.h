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

#include <QDateTime>
#include <QQmlListProperty>
#include <QString>
#include <QVariant>

#include "album/album-page.h"
#include "album/album-template.h"
#include "core/container-source.h"
#include "media/media-source.h"

/*!
 * \brief The AlbumTemplate class

 Note about page numbers: the first page (number 0) is the front cover.  It
 always shows up on the right side of the spread.  The first "content" page
 is number 1, and it shows up on the left.  There are always an even number
 of content pages.  The last page (after the last content page) is the back
 cover, which always shows up on the left.  You can use first/lastContentPage
 and total/contentPageCount to avoid off-by-one errors.

 Note about "current page number": the current page is the page on the left
 of the current spread.  This means that when the album is closed,
 currentPage is actually -1 (since the cover on the right is page 0).
 You can use first/lastValidCurrentPage to avoid off-by-one errors.
 */
class Album : public ContainerSource
{
  Q_OBJECT
  Q_PROPERTY(QString title READ title WRITE set_title NOTIFY title_altered)
  Q_PROPERTY(QString subtitle READ subtitle WRITE set_subtitle NOTIFY subtitle_altered)
  Q_PROPERTY(QDateTime creationDateTime READ creation_date_time
    NOTIFY creation_date_time_altered)
  Q_PROPERTY(QQmlListProperty<AlbumPage> contentPages READ qml_pages
    NOTIFY contentPagesAltered)
  Q_PROPERTY(QQmlListProperty<MediaSource> allMediaSources
    READ qml_all_media_sources NOTIFY album_contents_altered)
  Q_PROPERTY(int firstContentPage READ first_content_page NOTIFY pageCountAltered)
  Q_PROPERTY(int lastContentPage READ last_content_page NOTIFY pageCountAltered)
  Q_PROPERTY(int lastPopulatedContentPage READ last_populated_content_page NOTIFY pageCountAltered)
  Q_PROPERTY(int totalPageCount READ total_page_count NOTIFY pageCountAltered)
  Q_PROPERTY(int contentPageCount READ content_page_count NOTIFY pageCountAltered)
  Q_PROPERTY(int populatedContentPageCount READ populated_content_page_count NOTIFY pageCountAltered)
  Q_PROPERTY(int firstValidCurrentPage READ first_valid_current_page NOTIFY pageCountAltered)
  Q_PROPERTY(int lastValidCurrentPage READ last_valid_current_page NOTIFY pageCountAltered)
  Q_PROPERTY(int currentPage READ current_page WRITE set_current_page
    NOTIFY current_page_altered)
  Q_PROPERTY(bool closed READ is_closed WRITE set_closed NOTIFY closedAltered)
  Q_PROPERTY(QString coverNickname READ cover_nickname WRITE set_cover_nickname
    NOTIFY coverNicknameAltered)
  
 signals:
  void album_contents_altered();
  void creation_date_time_altered();
  void current_page_altered();
  void current_page_contents_altered();
  void title_altered();
  void subtitle_altered();
  void closedAltered();
  void contentPagesAltered();
  void pageCountAltered();
  void coverNicknameAltered();
  
 public:
  static const char *DEFAULT_TITLE;
  static const char *DEFAULT_SUBTITLE;
  static const int PAGES_PER_COVER;
  static const int FIRST_VALID_CURRENT_PAGE;

  Album(QObject * parent = 0);
  explicit Album(QObject * parent, AlbumTemplate* album_template);
  Album(QObject * parent, AlbumTemplate* album_template, const QString &title,
    const QString &subtitle, qint64 id, QDateTime creation_timestamp, bool closed,
    int current_page, const QString &cover_nickname);

  virtual ~Album();
  
  static void RegisterType();
  
  Q_INVOKABLE void addMediaSource(QVariant vmedia);
  Q_INVOKABLE QVariant addSelectedMediaSources(QVariant vmodel);
  Q_INVOKABLE void removeMediaSource(QVariant vmedia);
  Q_INVOKABLE void removeSelectedMediaSources(QVariant vmodel);
  Q_INVOKABLE QVariant getPage(int page) const;
  Q_INVOKABLE int getPageForMediaSource(QVariant vmedia) const;
  Q_INVOKABLE bool containsMedia(QVariant vmedia) const;
  Q_INVOKABLE bool containsAll(QVariant vContainerSource) const;
  
  const QString& title() const;
  void set_title(QString title);
  const QString& subtitle() const;
  void set_subtitle(QString subtitle);
  const QDateTime& creation_date_time() const;
  void set_creation_date_time(QDateTime timestamp);
  AlbumTemplate* album_template() const;
  bool is_closed() const;
  int total_page_count() const;
  int content_page_count() const;
  int populated_content_page_count() const;
  int first_content_page() const;
  int last_content_page() const;
  int last_populated_content_page() const;
  int first_valid_current_page() const;
  int last_valid_current_page() const;
  int current_page() const;
  void set_current_page(int page);
  void set_closed(bool closed);
  void set_id(qint64 id_);
  qint64 get_id();
  QString cover_nickname() const;
  void set_cover_nickname(QString name);
  
  SourceCollection* content_pages();
  
  AlbumPage* GetPage(int page) const;
  
  QQmlListProperty<AlbumPage> qml_pages();
  QQmlListProperty<MediaSource> qml_all_media_sources();
  
 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
  virtual void notify_current_page_altered();
  virtual void notify_closed_altered();
  virtual void notify_page_count_altered();
  virtual void notify_content_pages_altered();
  virtual void notify_current_page_contents_altered();
  virtual void notify_container_contents_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);

  int content_to_absolute_page(int content_page) const;
  int absolute_to_content_page(int absolute_page) const;
  
  void create_add_photos_page();
  
 private slots:
  void on_album_page_content_altered(const QSet<DataObject*>* added,
    const QSet<DataObject*>* removed);
  
 private:
  AlbumTemplate* album_template_;
  QString title_;
  QString subtitle_;
  QDateTime creation_date_time_;
  int current_page_; // Page number of the left page of the current spread.
  bool closed_;
  int populated_pages_count_;
  SourceCollection* content_pages_;
  QList<MediaSource*> all_media_sources_;
  QList<AlbumPage*> all_album_pages_;
  bool refreshing_container_;
  qint64 id_;
  QString cover_nickname_;
  
  void InitInstance();
};

QML_DECLARE_TYPE(Album)

#endif  // GALLERY_ALBUM_H_

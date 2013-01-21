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

#ifndef GALLERY_ALBUM_PAGE_H_
#define GALLERY_ALBUM_PAGE_H_

#include <QObject>
#include <QUrl>

#include "album/album-template-page.h"
#include "core/container-source.h"
#include "media/media-source.h"

class Album;

class AlbumPage : public ContainerSource {
  Q_OBJECT
  Q_PROPERTY(int pageNumber READ page_number NOTIFY page_number_changed);
  Q_PROPERTY(QUrl qmlRc READ qml_rc NOTIFY qml_rc_changed);
  Q_PROPERTY(QQmlListProperty<MediaSource> mediaSourceList
    READ qml_media_source_list NOTIFY media_source_list_changed);
  Q_PROPERTY(QVariant owner READ qml_owner NOTIFY owner_changed);
  
 signals:
  void page_number_changed();
  void qml_rc_changed();
  void media_source_list_changed();
  void owner_changed();
  
 public:
  AlbumPage(QObject * parent = 0);
  explicit AlbumPage(Album* owner);
  AlbumPage(Album* owner, int page_number, AlbumTemplatePage* template_page);
  
  static void RegisterType();
  
  int page_number() const;
  AlbumTemplatePage* template_page() const;
  QUrl qml_rc() const;

  QQmlListProperty<MediaSource> qml_media_source_list();
  QVariant qml_owner() const;
  
 protected:
  virtual void DestroySource(bool destroy_backing, bool as_orphan);
  
  virtual void notify_container_contents_altered(const QSet<DataObject *> *added,
    const QSet<DataObject *> *removed);
  
 private:
  Album* owner_;
  int page_number_;
  AlbumTemplatePage* template_page_;
  QList<MediaSource*> source_list_;
};

QML_DECLARE_TYPE(AlbumPage);

#endif  // GALLERY_ALBUM_PAGE_H_

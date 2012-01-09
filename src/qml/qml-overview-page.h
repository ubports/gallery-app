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
 * Lucas Beeler <lucas@yorba.org>
 */

#ifndef GALLERY_QML_OVERVIEW_PAGE_H_
#define GALLERY_QML_OVERVIEW_PAGE_H_

#include <QObject>
#include <QDeclarativeView>

#include "album/album.h"
#include "core/selectable-view-collection.h"
#include "media/media-source.h"
#include "qml/qml-album-collection-model.h"
#include "qml/qml-media-model.h"
#include "qml/qml-page.h"

class QmlOverviewPage : public QmlPage {
  Q_OBJECT
  
 signals:
  void photo_activated(MediaSource* media_source);
  void album_activated(Album* album);
  void create_album();
  
 public:
  QmlOverviewPage(QDeclarativeView* view);
  virtual ~QmlOverviewPage();
  
  QmlMediaModel* media_model() const;
  QmlAlbumCollectionModel* albums_model() const;
  
  virtual const char* qml_rc() const;
  
  virtual void PrepareContext();
  virtual void PageLoaded();
  void PrepareToEnter();
  
 private slots:
  void on_photo_activated(int photo_number);
  void on_photo_selection_toggled(int photo_number);
  void on_photos_unselect_all();
  void on_create_album_from_selected_photos();
  void on_album_activated(int album_number);
  void on_popup_album_picked(int album_number);
  
 private:
  SelectableViewCollection media_view_;
  SelectableViewCollection albums_view_;
  QmlMediaModel* media_model_;
  QmlAlbumCollectionModel* albums_model_;
};

#endif  // GALLERY_QML_OVERVIEW_PAGE_H_

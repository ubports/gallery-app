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

#ifndef GALLERY_QML_ALBUM_VIEWER_PAGE_H_
#define GALLERY_QML_ALBUM_VIEWER_PAGE_H_

#include <QObject>
#include <QDeclarativeView>

#include "album/album.h"
#include "core/selectable-view-collection.h"
#include "media/media-source.h"
#include "qml/qml-album-model.h"
#include "qml/qml-media-model.h"
#include "qml/qml-page.h"

class QmlAlbumViewerPage : public QmlPage {
  Q_OBJECT
  
 signals:
  void exit_viewer();
  void media_activated(MediaSource* media);
  void add_media_to_album(Album* album);
  
 public:
  QmlAlbumViewerPage(QDeclarativeView* view);
  virtual ~QmlAlbumViewerPage();
  
  virtual const char *qml_rc() const;
  
  virtual void PrepareContext();
  virtual void PageLoaded();
  void PrepareToEnter(Album* album);
  
  QmlMediaModel* media_model() const;
  
 private slots:
  void on_media_activated(int media_number);
  void on_media_selection_toggled(int media_number);
  void on_media_unselect_all();
  void on_popup_album_picked(int album_number);
  void on_create_album_from_selected_media();
  void on_add_to_album();
  
 private:
  Album* album_;
  QmlAlbumModel* album_model_;
  QmlMediaModel* media_model_;
  SelectableViewCollection* view_;
};

#endif  // GALLERY_QML_ALBUM_VIEWER_PAGE_H_

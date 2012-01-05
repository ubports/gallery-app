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

#ifndef GALLERY_ALBUM_VIEWER_H_
#define GALLERY_ALBUM_VIEWER_H_

#include <QObject>
#include <QDeclarativeView>

#include "album.h"
#include "media-source.h"
#include "qml-album-model.h"
#include "qml-media-model.h"
#include "qml-page.h"
#include "selectable-view-collection.h"

class AlbumViewer : public QmlPage {
  Q_OBJECT
  
 signals:
  void exit_viewer();
  void media_activated(MediaSource* media);
  void add_media_to_album(Album* album);
  
 public:
  AlbumViewer(QDeclarativeView* view);
  virtual ~AlbumViewer();
  
  virtual const char *qml_rc() const;
  
  virtual void PrepareContext();
  virtual void PageLoaded();
  void PrepareToEnter(Album* album);
  
  QmlMediaModel* media_model() const;
  
 private slots:
  void on_media_activated(int media_number);
  void on_add_to_album();
  
 private:
  Album* album_;
  QmlAlbumModel* album_model_;
  QmlMediaModel* media_model_;
  SelectableViewCollection* view_;
};

#endif  // GALLERY_ALBUM_VIEWER_H_

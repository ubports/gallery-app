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
#include "qml-album-model.h"
#include "qml-media-model.h"
#include "qml-page.h"
#include "selectable-view-collection.h"

class AlbumViewer : public QmlPage {
  Q_OBJECT
  
 signals:
  void exit_viewer();
  
 public:
  AlbumViewer(QDeclarativeView* view);
  virtual ~AlbumViewer();
  
  virtual const char *qml_rc() const;
  
  void Prepare(Album* album);
  virtual void SwitchingTo();
  virtual void SwitchingFrom();
  
 private:
  QmlAlbumModel* album_model_;
  QmlMediaModel* media_model_;
  SelectableViewCollection* view_;
};

#endif  // GALLERY_ALBUM_VIEWER_H_

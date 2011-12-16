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

#include "album-viewer.h"

AlbumViewer::AlbumViewer(QDeclarativeView* view)
  : QmlPage(view), album_model_(NULL), media_model_(NULL), view_(NULL) {
}

AlbumViewer::~AlbumViewer() {
  delete media_model_;
  delete album_model_;
  delete view_;
}

const char* AlbumViewer::qml_rc() const {
  return "qrc:/rc/qml/AlbumViewer.qml";
}

void AlbumViewer::Prepare(Album* album) {
  Q_ASSERT(view_ == NULL);
  view_ = new SelectableViewCollection();
  view_->MonitorDataCollection(album->contained(), NULL, false);
  
  Q_ASSERT(album_model_ == NULL);
  album_model_ = new QmlAlbumModel(NULL);
  album_model_->Init(album);
  
  Q_ASSERT(media_model_ == NULL);
  media_model_ = new QmlMediaModel(NULL);
  media_model_->Init(view_);
  
  SetContextProperty("context_album_model", album_model_);
  SetContextProperty("context_media_model", media_model_);
  SetContextProperty("context_start_index", album->current_page());
}

void AlbumViewer::SwitchingTo() {
  Connect("album_viewer", SIGNAL(exit_viewer()), this, SIGNAL(exit_viewer()));
}

void AlbumViewer::SwitchingFrom() {
  delete media_model_;
  media_model_ = NULL;
  
  delete album_model_;
  album_model_ = NULL;
  
  delete view_;
  view_ = NULL;
}

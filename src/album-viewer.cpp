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

AlbumViewer::AlbumViewer()
  : agent_(NULL), model_(NULL) {
}

AlbumViewer::~AlbumViewer() {
  delete agent_;
  delete model_;
}

const char* AlbumViewer::qml_rc() const {
  return "qrc:/rc/qml/AlbumViewer.qml";
}

void AlbumViewer::Prepare(QDeclarativeView* view, Album* album) {
  Q_ASSERT(model_ == NULL);
  model_ = new QmlAlbumModel(NULL);
  model_->Init(album);
  
  AlbumViewerAgent::Prepare(view, model_, album->current_page());
}

void AlbumViewer::SwitchingTo(QDeclarativeView* view) {
  Q_ASSERT(agent_ == NULL);
  agent_ = new AlbumViewerAgent(view);
  
  QObject::connect(agent_, SIGNAL(exit_viewer()), this, SIGNAL(exit_viewer()));
}

void AlbumViewer::SwitchingFrom(QDeclarativeView* view) {
  delete agent_;
  agent_ = NULL;
  
  delete model_;
  model_ = NULL;
}

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

#include "album-viewer-agent.h"

#include <QDeclarativeItem>

AlbumViewerAgent::AlbumViewerAgent(QDeclarativeView* view)
  : QmlAgent(view) {
  
  QDeclarativeItem* viewer = FindChild("album_viewer");
  
  QObject::connect(viewer, SIGNAL(exit_viewer()), this, SIGNAL(exit_viewer()));
}

void AlbumViewerAgent::Prepare(QDeclarativeView* view, QmlAlbumModel* model,
  int start_index) {
  SetContextProperty(view, "context_album_model", model);
  SetContextProperty(view, "context_start_index", start_index);
}

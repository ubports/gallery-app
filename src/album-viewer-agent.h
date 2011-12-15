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

#ifndef GALLERY_ALBUM_VIEWER_AGENT_H_
#define GALLERY_ALBUM_VIEWER_AGENT_H_

#include <QObject>
#include <QDeclarativeView>

#include "qml-agent.h"
#include "qml-album-model.h"

class AlbumViewerAgent : public QmlAgent {
  Q_OBJECT
  
 signals:
  void exit_viewer();
  
 public:
  static void Prepare(QDeclarativeView* view, QmlAlbumModel* model,
    int start_index);
  
  explicit AlbumViewerAgent(QDeclarativeView* view);
};

#endif  // GALLERY_ALBUM_VIEWER_AGENT_H_

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
#include "album-viewer-agent.h"
#include "qml-album-model.h"
#include "qml-page.h"

class AlbumViewer : public QmlPage {
  Q_OBJECT
  
 signals:
  void exit_viewer();
  
 public:
  AlbumViewer();
  virtual ~AlbumViewer();
  
  virtual const char *qml_rc() const;
  
  void Prepare(QDeclarativeView* view, Album* album);
  virtual void SwitchingTo(QDeclarativeView* view);
  virtual void SwitchingFrom(QDeclarativeView* view);
  
 private:
  AlbumViewerAgent* agent_;
  QmlAlbumModel* model_;
};

#endif  // GALLERY_ALBUM_VIEWER_H_

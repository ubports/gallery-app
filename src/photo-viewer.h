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

#ifndef GALLERY_PHOTO_VIEWER_H_
#define GALLERY_PHOTO_VIEWER_H_

#include <QObject>
#include <QDeclarativeView>

#include "photo-viewer-agent.h"
#include "photo.h"
#include "qml-media-model.h"

class PhotoViewer : public QObject {
  Q_OBJECT
  
signals:
  void exit_viewer();
  
public:
  PhotoViewer();
  virtual ~PhotoViewer();
  
  // returned path is a relative path, not an absolute one
  const char* qml_file_path() const;
  
  void Prepare(QDeclarativeView* view, QmlMediaModel* model, Photo* start);
  void SwitchingTo(QDeclarativeView* view);
  void SwitchingFrom(QDeclarativeView* view);
  
private:
  PhotoViewerAgent* agent_;
  QmlMediaModel* model_;
};

#endif  // GALLERY_PHOTO_VIEWER_H_

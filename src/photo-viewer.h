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

#include "photo.h"
#include "qml-media-model.h"
#include "qml-page.h"

class PhotoViewer : public QmlPage {
  Q_OBJECT
  
 signals:
  void exit_viewer();
  
 public:
  PhotoViewer(QDeclarativeView* view);
  virtual ~PhotoViewer();
  
  // returned path is a relative path, not an absolute one
  virtual const char* qml_rc() const;
  
  void Prepare(QmlMediaModel* model, Photo* start);
  virtual void SwitchingTo();
  virtual void SwitchingFrom();
};

#endif  // GALLERY_PHOTO_VIEWER_H_

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

#ifndef GALLERY_GUI_CONTROLLER_H_
#define GALLERY_GUI_CONTROLLER_H_

#include <QObject>
#include <QDeclarativeView>
#include <QPointer>

#include "media-source.h"
#include "media-collection.h"
#include "checkerboard.h"
#include "photo-viewer.h"

class GuiController : public QObject {
  Q_OBJECT
  
public:
  explicit GuiController(const QDir& path);
  virtual ~GuiController();
    
private slots:
  void on_checkerboard_media_object_activated(MediaSource* media_source);
  void on_photo_viewer_exited();
  void on_power_off();
    
private:
  QDeclarativeView* view_;
  QPointer<QObject> loader_;
  QPointer<QObject> tablet_surface_;
  
  MediaCollection* library_;
  Checkerboard* checkerboard_;
  PhotoViewer* photo_viewer_;
  
  void SetSource(const char* path);
};

#endif  // GALLERY_GUI_CONTROLLER_H_

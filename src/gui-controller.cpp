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

#include "gui-controller.h"

#include <cstdlib>

#include <QObject>
#include <QString>
#include <QDir>
#include <QDeclarativeItem>

GuiController::GuiController(const QDir &path) {
  view_ = new QDeclarativeView();
  view_->setSource(QUrl::fromLocalFile("qml/TabletSurface.qml"));
  
  tablet_surface_ = qobject_cast<QObject*>(view_->rootObject());
  Q_ASSERT(!tablet_surface_.isNull() &&
    tablet_surface_->objectName() == "tablet_surface");
  
  QObject::connect(tablet_surface_, SIGNAL(power_off()), this,
    SLOT(on_power_off()));
  
  loader_ = tablet_surface_->findChild<QObject*>("loader");
  Q_ASSERT(loader_ != NULL);
  
  library_ = new MediaCollection(path);
  
  checkerboard_ = new Checkerboard(library_, NULL);
  photo_viewer_ = new PhotoViewer();
  
  QObject::connect(checkerboard_, SIGNAL(activated(MediaObject*)), this,
    SLOT(on_checkerboard_media_object_activated(MediaObject*)));
  
  QObject::connect(photo_viewer_, SIGNAL(exit_viewer()), this,
    SLOT(on_photo_viewer_exited()));
  
  checkerboard_->Prepare(view_);
  SetSource(checkerboard_->qml_file_path());
  checkerboard_->SwitchingTo(view_);
  
  view_->show();
}

GuiController::~GuiController() {
  delete checkerboard_;
  delete photo_viewer_;
  
  delete library_;
}

void GuiController::on_checkerboard_media_object_activated(MediaObject* media_object) {
  Photo* photo = qobject_cast<Photo*>(media_object);
  if (photo == NULL) {
    qDebug("Non-photo object activated");
    
    return;
  }
  
  checkerboard_->SwitchingFrom(view_);
  
  photo_viewer_->Prepare(view_, checkerboard_->model(), photo);
  SetSource(photo_viewer_->qml_file_path());
  photo_viewer_->SwitchingTo(view_);

  view_->show();
}

void GuiController::on_power_off() {
  std::exit(0);
}

void GuiController::on_photo_viewer_exited() {
  photo_viewer_->SwitchingFrom(view_);

  checkerboard_->Prepare(view_);
  SetSource(checkerboard_->qml_file_path());
  checkerboard_->SwitchingTo(view_);

  view_->show();
}

void GuiController::SetSource(const char *path) {
  if (loader_ != NULL)
    loader_->setProperty("source", QUrl::fromLocalFile(path));
}

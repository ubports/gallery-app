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
#include <QUrl>
#include <QGLWidget>

GuiController::GuiController(const QDir &path) {
  MediaCollection::InitInstance(path);
  
  //
  // TableSurface (master container holding other pages)
  //
  
  // Enable OpenGL backing
  QGLFormat format = QGLFormat::defaultFormat();
  format.setSampleBuffers(false);
  QGLWidget *glWidget = new QGLWidget(format);
  
  view_ = new QDeclarativeView();
  view_->setSource(QUrl("qrc:/rc/qml/TabletSurface.qml"));
  view_->setViewport(glWidget);
  
  tablet_surface_ = qobject_cast<QObject*>(view_->rootObject());
  Q_ASSERT(!tablet_surface_.isNull() &&
    tablet_surface_->objectName() == "tablet_surface");
  
  QObject::connect(tablet_surface_, SIGNAL(power_off()), this,
    SLOT(on_power_off()));
  
  loader_ = tablet_surface_->findChild<QObject*>("loader");
  Q_ASSERT(loader_ != NULL);
  
  //
  // Overview (Photos / Albums)
  
  overview_ = new Overview();
  
  QObject::connect(overview_, SIGNAL(photo_activated(MediaSource*)), this,
    SLOT(on_media_object_activated(MediaSource*)));
  
  QObject::connect(overview_, SIGNAL(album_activated(Album*)), this,
    SLOT(on_album_activated(Album*)));
  
  //
  // PhotoViewer
  //
  
  photo_viewer_ = new PhotoViewer();
  
  QObject::connect(photo_viewer_, SIGNAL(exit_viewer()), this,
    SLOT(on_photo_viewer_exited()));
  
  //
  // AlbumViewer
  //
  
  album_viewer_ = new AlbumViewer();
  
  QObject::connect(album_viewer_, SIGNAL(exit_viewer()), this,
    SLOT(on_exit_album_viewer()));
  
  // start with Overview
  overview_->Prepare(view_);
  SetSource(overview_->qml_rc());
  overview_->SwitchingTo(view_);
  
  view_->show();
}

GuiController::~GuiController() {
  delete overview_;
  delete photo_viewer_;
  delete album_viewer_;
}

void GuiController::on_media_object_activated(MediaSource* media_source) {
  Photo* photo = qobject_cast<Photo*>(media_source);
  if (photo == NULL) {
    qDebug("Non-photo object activated");
    
    return;
  }
  
  overview_->SwitchingFrom(view_);
  
  photo_viewer_->Prepare(view_, overview_->photos_model(), photo);
  SetSource(photo_viewer_->qml_rc());
  photo_viewer_->SwitchingTo(view_);

  view_->show();
}

void GuiController::on_power_off() {
  std::exit(0);
}

void GuiController::on_album_activated(Album* album) {
  overview_->SwitchingFrom(view_);
  
  album_viewer_->Prepare(view_, album);
  SetSource(album_viewer_->qml_rc());
  album_viewer_->SwitchingTo(view_);
  
  view_->show();
}

void GuiController::on_photo_viewer_exited() {
  photo_viewer_->SwitchingFrom(view_);
  
  overview_->Prepare(view_);
  SetSource(overview_->qml_rc());
  overview_->SwitchingTo(view_);
  
  view_->show();
}

void GuiController::on_exit_album_viewer() {
  album_viewer_->SwitchingFrom(view_);
  
  overview_->Prepare(view_);
  SetSource(overview_->qml_rc());
  overview_->SwitchingTo(view_);
  
  view_->show();
}

void GuiController::SetSource(const char *path) {
  if (loader_ != NULL)
    loader_->setProperty("source", QUrl(path));
}

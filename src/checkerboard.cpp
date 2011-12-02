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

#include "checkerboard.h"

#include <QDeclarativeContext>
#include <QDeclarativeView>

Checkerboard::Checkerboard(MediaCollection* media, SourceFilter filter)
  : agent_(NULL) {
  view_.MonitorSourceCollection(media, filter);
  
  model_ = new QmlMediaModel(NULL);
  model_->Init(&view_);
}

Checkerboard::~Checkerboard() {
  delete agent_;
  delete model_;
}

QmlMediaModel* Checkerboard::model() const {
  return model_;
}

const char* Checkerboard::qml_file_path() const {
  return "qml/Checkerboard.qml";
}

void Checkerboard::Prepare(QDeclarativeView* view) {
  view->rootContext()->setContextProperty("gridModel", model_);
}

void Checkerboard::SwitchingTo(QDeclarativeView* view) {
  Q_ASSERT(agent_ == NULL);
  agent_ = new CheckerboardAgent(view);
  
  QObject::connect(agent_, SIGNAL(activated(int)), this,
    SLOT(on_activated(int)));
  
  QObject::connect(agent_, SIGNAL(selection_toggled(int)), this,
    SLOT(on_selection_toggled(int)));
  
  QObject::connect(agent_, SIGNAL(unselect_all()), this,
    SLOT(on_unselect_all()));
}

void Checkerboard::SwitchingFrom(QDeclarativeView* view) {
  delete agent_;
  agent_ = NULL;
}

void Checkerboard::on_activated(int media_number) {
  MediaObject* media_object = qobject_cast<MediaObject*>(
    view_.FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    emit activated(media_object);
  else
    qDebug("Unable to locate activated photo #%d", media_number);
}

void Checkerboard::on_selection_toggled(int media_number) {
  MediaObject* media_object = qobject_cast<MediaObject*>(
    view_.FindByNumber((MediaNumber) media_number));
  if (media_object != NULL)
    view_.ToggleSelect(media_object);
  else
    qDebug("Unable to located toggled photo #%d", media_number);
}

void Checkerboard::on_unselect_all() {
  view_.UnselectAll();
}

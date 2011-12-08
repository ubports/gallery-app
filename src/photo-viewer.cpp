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

#include "photo-viewer.h"

#include <QObject>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QFile>
#include <QString>

#include "qml-media-model.h"
#include "photo.h"

PhotoViewer::PhotoViewer()
  : agent_(NULL), model_(NULL) {
}

PhotoViewer::~PhotoViewer() {
  delete agent_;
}

const char* PhotoViewer::qml_file_path() const {
  return "qrc:/rc/qml/PhotoViewer.qml";
}

void PhotoViewer::Prepare(QDeclarativeView *view, QmlMediaModel *model,
  Photo* start) {
  view->rootContext()->setContextProperty("viewer_model", model);
  view->rootContext()->setContextProperty("viewer_current_index",
    model->BackingViewCollection()->IndexOf(*start));
}

void PhotoViewer::SwitchingTo(QDeclarativeView* view) {
  Q_ASSERT(agent_ == NULL);
  agent_ = new PhotoViewerAgent(view);
  
  QObject::connect(agent_, SIGNAL(exit_pressed()), this,
    SIGNAL(exit_viewer()));
}

void PhotoViewer::SwitchingFrom(QDeclarativeView* view) {
  delete agent_;
  agent_ = NULL;
}

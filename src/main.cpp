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

#include <QApplication>
#include <QDir>
#include <QString>

#include "qml/qml-album-model.h"
#include "qml/qml-album-collection-model.h"
#include "qml/qml-media-model.h"
#include "qml/qml-media-source.h"
#include "ui-controller.h"

int main(int argc, char *argv[]) {
  // NOTE: This *must* be called prior to QApplication's ctor.
  QApplication::setGraphicsSystem("opengl");
  
  QApplication app(argc, argv);
  
  // QML Declarative types must be registered before use; if we use a lot of
  // these, we may want a more formal registration system
  QmlAlbumModel::RegisterType();
  QmlAlbumCollectionModel::RegisterType();
  QmlMediaModel::RegisterType();
  QmlMediaSource::RegisterType();
  
  QDir path(argc > 1 ? QString(argv[1]) : QDir::homePath() + "/Pictures");
  qDebug("Opening %s...", qPrintable(path.path()));
  
  UIController ui(path);
  
  return app.exec();
}

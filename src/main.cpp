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
#include <QString>
#include <QDir>
#include <QDeclarativeView>

#include "gui-controller.h"
#include "qml-media-model.h"

int main(int argc, char *argv[]) {
  // This is a significant performance win ... "opengl" doesn't appear to be
  // available in 4.7 without a special build of Qt, but if available on final
  // device, probably should be configured to use that.
  //
  // NOTE: This *must* be called prior to QApplication's ctor.
  QApplication::setGraphicsSystem("raster");
  
  QApplication app(argc, argv);
  
  // QML Declarative types must be registered before use; if we use a lot of
  // these, we may want a more formal registration system
  QmlMediaModel::RegisterType();
  
  QDir path(argc > 1 ? QString(argv[1]) : QDir::homePath() + "/Pictures");
  qDebug("Opening %s...", qPrintable(path.path()));
  
  GuiController gui(path);
  
  return app.exec();
}

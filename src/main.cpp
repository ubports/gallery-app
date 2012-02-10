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
#include <QGLWidget>
#include <QString>
#include <QUrl>

#include "album/album.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "album/album-page.h"
#include "event/event.h"
#include "event/event-collection.h"
#include "media/media-collection.h"
#include "media/media-source.h"
#include "media/preview-manager.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/qml-album-collection-model.h"
#include "qml/qml-album-page-model.h"
#include "qml/qml-event-collection-model.h"
#include "qml/qml-event-overview-model.h"
#include "qml/qml-event-marker.h"
#include "qml/qml-media-collection-model.h"
#include "qml/qml-stack.h"

int main(int argc, char *argv[]) {
  // NOTE: This *must* be called prior to QApplication's ctor.
  QApplication::setGraphicsSystem("opengl");
  
  QApplication app(argc, argv);
  
  //
  // QML Declarative types must be registered before use
  //
  // TODO: Use QML Plugins to automate this
  //
  
  Album::RegisterType();
  AlbumPage::RegisterType();
  Event::RegisterType();
  MediaSource::RegisterType();
  QmlAlbumCollectionModel::RegisterType();
  QmlAlbumPageModel::RegisterType();
  QmlEventCollectionModel::RegisterType();
  QmlEventOverviewModel::RegisterType();
  QmlEventMarker::RegisterType();
  QmlMediaCollectionModel::RegisterType();
  QmlStack::RegisterType();
  
  //
  // Library is currently only loaded from ~/Pictures (no subdirectory
  // traversal)
  //
  
  QDir path(argc > 1 ? QString(argv[1]) : QDir::homePath() + "/Pictures");
  
  qDebug("Opening %s...", qPrintable(path.path()));
  
  // Not in alpha-order because initialization order is important here
  // TODO: Need to use an initialization system that deals with init order
  // issues
  MediaCollection::Init(path);
  AlbumCollection::Init();
  EventCollection::Init();
  PreviewManager::Init();
  AlbumDefaultTemplate::Init();
  GalleryStandardImageProvider::Init();
  
  qDebug("Opened %s", qPrintable(path.path()));
  
  //
  // Create the master QDeclarativeView that all the pages will operate within
  // using the OpenGL backing and load the root container
  //
  
  QGLFormat format = QGLFormat::defaultFormat();
  format.setSampleBuffers(false);
  QGLWidget *gl_widget = new QGLWidget(format);
  
  QDeclarativeView view;
  view.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
    GalleryStandardImageProvider::instance());
  view.setSource(QUrl("qrc:/rc/qml/TabletSurface.qml"));
  view.setViewport(gl_widget);
  QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
  
  view.show();
  
  return app.exec();
}

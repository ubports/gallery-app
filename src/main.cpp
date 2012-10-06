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
#include <QString>
#include <QQuickView>

#include "album/album.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "album/album-page.h"
#include "database/database.h"
#include "event/event.h"
#include "event/event-collection.h"
#include "media/media-collection.h"
#include "media/media-source.h"
#include "media/preview-manager.h"
#include "photo/photo-metadata.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/qml-album-collection-model.h"
#include "qml/qml-event-collection-model.h"
#include "qml/qml-event-overview-model.h"
#include "qml/qml-media-collection-model.h"
#include "qml/qml-stack.h"
#include "util/resource.h"

const int APP_GRIDUNIT = 8;

// Path to database, relative to pictures path.
const QString database_path = ".database";

int main(int argc, char *argv[]) {
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
  QmlEventCollectionModel::RegisterType();
  QmlEventOverviewModel::RegisterType();
  QmlMediaCollectionModel::RegisterType();
  QmlStack::RegisterType();
  
  QStringList args = app.arguments();

  QHash<QString, QSize> form_factors;
  form_factors.insert("desktop", QSize(160, 100)); // In BGU.
  form_factors.insert("tablet", QSize(160, 100));
  form_factors.insert("phone", QSize(71, 40));
  form_factors.insert("sidebar", QSize(71, 40));

  QString form_factor = "desktop";
  foreach(const QString& test, form_factors.keys()) {
    QString arg_test = QString("--%1").arg(test);
    if (args.contains(arg_test)) {
      args.removeAll(arg_test);
      form_factor = test;
    }
  }

  bool is_portrait = false;
  if (args.contains("--landscape")) {
    args.removeAll("--landscape");
    is_portrait = false;
  }
  if (args.contains("--portrait")) {
    args.removeAll("--portrait");
    is_portrait = true;
  }

  //
  // Library is currently only loaded from ~/Pictures (no subdirectory
  // traversal)
  //
  
  QDir pictures_path(args.count() > 1 ? args.at(1) : QDir::homePath() + "/Pictures");
  
  qDebug("Opening %s...", qPrintable(pictures_path.path()));
  
  QDir db_dir(pictures_path);
  db_dir.mkdir(database_path);
  db_dir.cd(database_path);
  
  // Not in alpha-order because initialization order is important here
  // TODO: Need to use an initialization system that deals with init order
  // issues
  PhotoMetadata::Init(); // must init before loading photos
  Resource::Init(app.applicationDirPath(), INSTALL_PREFIX);
  Database::Init(db_dir, &app);
  Database::instance()->get_media_table()->verify_files();
  AlbumDefaultTemplate::Init();
  MediaCollection::Init(pictures_path); // only init after db
  AlbumCollection::Init(); // only init after media collection
  EventCollection::Init();
  PreviewManager::Init();
  GalleryStandardImageProvider::Init();
  
  qDebug("Opened %s", qPrintable(pictures_path.path()));
  
  //
  // Create the master QDeclarativeView that all the pages will operate within
  // using the OpenGL backing and load the root container
  //
  
  QQuickView view;
  view.setWindowTitle("Gallery");

  QSize size = form_factors[form_factor];
  if (is_portrait)
    size.transpose();

  // Only the desktop is resizable.
  if (form_factor == "desktop") {
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setMinimumSize(QSize(60 * APP_GRIDUNIT, 60 * APP_GRIDUNIT));
  } else {
    view.setResizeMode(QQuickView::SizeViewToRootObject);
  }

  view.engine()->rootContext()->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
  view.engine()->rootContext()->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
  view.engine()->rootContext()->setContextProperty("FORM_FACTOR", QVariant(form_factor));
  view.engine()->rootContext()->setContextProperty("GRIDUNIT", QVariant(APP_GRIDUNIT));

  view.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
    GalleryStandardImageProvider::instance());
  view.setSource(Resource::instance()->get_rc_url("qml/GalleryApplication.qml"));
  QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
  
  view.show();
  
  return app.exec();
}

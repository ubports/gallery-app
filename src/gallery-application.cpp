/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Charles Lindsay <chaz@yorba.org>
 */

#include <QDir>
#include <QGLWidget>
#include <QString>
#include <QUrl>
#include <QString>
#include <QQuickItem>

#include "gallery-application.h"
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

GalleryApplication::GalleryApplication(int& argc, char** argv) :
    QApplication(argc, argv), form_factor_("desktop"), is_portrait_(false),
    view_() {

  form_factors_.insert("desktop", QSize(160, 100)); // In BGU.
  form_factors_.insert("tablet", QSize(160, 100));
  form_factors_.insert("phone", QSize(71, 40));
  form_factors_.insert("sidebar", QSize(71, 40));

  pictures_dir_ = QDir(QString("%1/Pictures").arg(QDir::homePath()));

  register_qml();
  process_args();
  init_common();
}

int GalleryApplication::exec() {
  create_view();

  // Delay init_collections() so the main loop is running before it kicks off.
  QTimer::singleShot(0, this, SLOT(start_init_collections()));

  return QApplication::exec();
}

void GalleryApplication::register_qml() {
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
}

void GalleryApplication::process_args() {
  QStringList args = arguments();

  foreach(const QString& test, form_factors_.keys()) {
    QString arg_test = QString("--%1").arg(test);
    if (args.contains(arg_test)) {
      args.removeAll(arg_test);
      form_factor_ = test;
    }
  }

  if (args.contains("--landscape")) {
    args.removeAll("--landscape");
    is_portrait_ = false;
  }
  if (args.contains("--portrait")) {
    args.removeAll("--portrait");
    is_portrait_ = true;
  }

  if (args.count() > 1)
    pictures_dir_ = QDir(args.takeFirst());
}

void GalleryApplication::init_common() {
  // These need to be initialized before create_view() or init_collections().
  Resource::Init(applicationDirPath(), INSTALL_PREFIX);
  GalleryStandardImageProvider::Init();
}

void GalleryApplication::create_view() {
  //
  // Create the master QDeclarativeView that all the pages will operate within
  // using the OpenGL backing and load the root container
  //

  view_.setWindowTitle("Gallery");

  QSize size = form_factors_[form_factor_];
  if (is_portrait_)
    size.transpose();

  // Only the desktop is resizable.
  if (form_factor_ == "desktop") {
    view_.setResizeMode(QQuickView::SizeRootObjectToView);
    view_.setMinimumSize(QSize(60 * GRIDUNIT, 60 * GRIDUNIT));
  } else {
    view_.setResizeMode(QQuickView::SizeViewToRootObject);
  }

  view_.engine()->rootContext()->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
  view_.engine()->rootContext()->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
  view_.engine()->rootContext()->setContextProperty("FORM_FACTOR", QVariant(form_factor_));
  view_.engine()->rootContext()->setContextProperty("GRIDUNIT", QVariant(GRIDUNIT));

  view_.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
    GalleryStandardImageProvider::instance());
  view_.setSource(Resource::instance()->get_rc_url("qml/GalleryApplication.qml"));
  QObject::connect(view_.engine(), SIGNAL(quit()), this, SLOT(quit()));

  // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
  QObject* rootObject = dynamic_cast<QObject*>(view_.rootObject());
  QObject::connect(this, SIGNAL(media_loaded()), rootObject, SLOT(onLoaded()));

  view_.show();
}

void GalleryApplication::init_collections() {
  //
  // Library is currently only loaded from pictures_dir_ (default ~/Pictures),
  // no subdirectory traversal)
  //

  qDebug("Opening %s...", qPrintable(pictures_dir_.path()));

  // Not in alpha-order because initialization order is important here
  // TODO: Need to use an initialization system that deals with init order
  // issues
  PhotoMetadata::Init(); // must init before loading photos
  Database::Init(pictures_dir_, this);
  Database::instance()->get_media_table()->verify_files();
  AlbumDefaultTemplate::Init();
  MediaCollection::Init(pictures_dir_); // only init after db
  AlbumCollection::Init(); // only init after media collection
  EventCollection::Init();
  PreviewManager::Init();

  qDebug("Opened %s", qPrintable(pictures_dir_.path()));

  emit media_loaded();
}

void GalleryApplication::start_init_collections() {
  init_collections();
}

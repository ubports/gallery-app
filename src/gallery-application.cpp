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
#include <QGLWidget>
#include <QString>
#include <QUrl>
#include <QString>
#include <QQuickItem>
#include <cstdlib>
#include <QProcess>

#include "gallery-application.h"
#include "album/album.h"
#include "album/album-page.h"
#include "database/database.h"
#include "event/event.h"
#include "media/media-source.h"
#include "photo/photo-metadata.h"
#include "photo/photo.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include "qml/qml-album-collection-model.h"
#include "qml/qml-event-collection-model.h"
#include "qml/qml-event-overview-model.h"
#include "qml/qml-media-collection-model.h"
#include "qml/qml-stack.h"
#include "util/resource.h"
#include "util/sharefile.h"
#include "core/gallery-manager.h"

GalleryApplication::GalleryApplication(int& argc, char** argv)
  : QApplication(argc, argv),
    form_factor_("desktop"),
    log_image_loading_(false),
    view_(),
    monitor_(NULL) {

  bgu_size_ = QProcessEnvironment::systemEnvironment().value("GRID_UNIT_PX", "8").toInt();
  if (bgu_size_ <= 0)
    bgu_size_ = 8;
  
  timer_.start();
  form_factors_.insert("desktop", QSize(160, 100)); // In BGU.
  form_factors_.insert("tablet", QSize(160, 100));
  form_factors_.insert("phone", QSize(71, 40));
  form_factors_.insert("sidebar", QSize(71, 40));

  register_qml();

  GalleryManager::GetInstance();
}

GalleryApplication::~GalleryApplication() {
  delete monitor_;
}

int GalleryApplication::exec() {
  create_view();

  // Delay init_collections() so the main loop is running before it kicks off.
  QTimer::singleShot(0, this, SLOT(start_init_collections()));

  return QApplication::exec();
}

bool GalleryApplication::run_command(const QString &cmd, const QString &arg) {
  return QProcess::startDetached(cmd, QStringList(arg));
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
  ShareFile::RegisterType();
}

void GalleryApplication::create_view() {
  //
  // Create the master QDeclarativeView that all the pages will operate within
  // using the OpenGL backing and load the root container
  //

  view_.setTitle("Gallery");

  QSize size = form_factors_[form_factor_];
  if (GalleryManager::GetInstance()->is_portrait())
    size.transpose();

  view_.setResizeMode(QQuickView::SizeRootObjectToView);
  if (form_factor_ == "desktop") {
    view_.setMinimumSize(QSize(60 * bgu_size_, 60 * bgu_size_));
  }
  
  view_.engine()->rootContext()->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
  view_.engine()->rootContext()->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
  view_.engine()->rootContext()->setContextProperty("FORM_FACTOR", QVariant(form_factor_));
  
  // Set ourselves up to expose functionality to run external commands from QML...
  view_.engine()->rootContext()->setContextProperty("APP", this);

  view_.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
    GalleryManager::GetInstance()->gallery_standard_image_provider());
  view_.engine()->addImageProvider(GalleryThumbnailImageProvider::PROVIDER_ID,
    GalleryManager::GetInstance()->gallery_thumbnail_image_provider());
  view_.setSource(GalleryManager::GetInstance()->resource()->get_rc_url("qml/GalleryApplication.qml"));
  QObject::connect(view_.engine(), SIGNAL(quit()), this, SLOT(quit()));

  // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
  QObject* rootObject = dynamic_cast<QObject*>(view_.rootObject());
  QObject::connect(this, SIGNAL(media_loaded()), rootObject, SLOT(onLoaded()));

  if (GalleryManager::GetInstance()->is_fullscreen())
    view_.showFullScreen();
  else
    view_.show();
}

void GalleryApplication::init_collections() {
  GalleryManager::GetInstance()->post_init();

  emit media_loaded();

  // start the file monitor so that the collection contents will be updated as
  // new files arrive
  monitor_ = new MediaMonitor(GalleryManager::GetInstance()->pictures_dir().path());
  QObject::connect(monitor_, SIGNAL(media_item_added(QFileInfo)), this,
    SLOT(on_media_item_added(QFileInfo)));

  if (GalleryManager::GetInstance()->startup_timer())
    qDebug() << "Startup took" << timer_.elapsed() << "milliseconds";
}

void GalleryApplication::start_init_collections() {
  init_collections();
}

GalleryApplication* GalleryApplication::instance() {
    return static_cast<GalleryApplication*>(qApp);
}

void GalleryApplication::setObjectOwnership(QObject* object, QQmlEngine::ObjectOwnership ownership) {
  view_.engine()->setObjectOwnership(object, ownership);
}

void GalleryApplication::on_media_item_added(QFileInfo item_info) {
  Photo* new_photo = Photo::Fetch(item_info);
  
  if (new_photo)
    GalleryManager::GetInstance()->media_collection()->Add(new_photo);
}

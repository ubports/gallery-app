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
#include <cstdlib>

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
#include <QProcess>

GalleryApplication* GalleryApplication::instance_ = NULL;

GalleryApplication::GalleryApplication(int& argc, char** argv) :
    QApplication(argc, argv), form_factor_("desktop"), is_portrait_(false),
    is_fullscreen_(false), view_(), startup_timer_(false), log_image_loading_(false),
    monitor_(NULL) {
  
  bgu_size_ = QProcessEnvironment::systemEnvironment().value("GRID_UNIT_PX", "8").toInt();
  if (bgu_size_ <= 0)
    bgu_size_ = 8;
  
  timer_.start();
  form_factors_.insert("desktop", QSize(160, 100)); // In BGU.
  form_factors_.insert("tablet", QSize(160, 100));
  form_factors_.insert("phone", QSize(71, 40));
  form_factors_.insert("sidebar", QSize(71, 40));

  pictures_dir_ = QDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

  register_qml();
  process_args();
  init_common();
  
  // only set instance_ variable at end of constructor, to ensure it's not
  // accessed prior to full construction
  Q_ASSERT(instance_ == NULL);
  instance_ = this;
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

void GalleryApplication::usage(bool error) {
  QTextStream out(error ? stderr : stdout);
  out << "Usage: gallery [options] [pictures_dir]" << endl;
  out << "Options:" << endl;
  out << "  --landscape\trun in landscape orientation (default)" << endl;
  out << "  --portrait\trun in portrait orientation" << endl;
  out << "  --fullscreen\trun fullscreen" << endl;
  foreach (const QString& form_factor, form_factors_.keys())
    out << "  --" << form_factor << "\trun in " << form_factor << " form factor" << endl;
  out << "  --startup-timer\n\t\tdebug-print startup time" << endl;
  out << "  --log-image-loading\n\t\tlog image loading" << endl;
  out << "pictures_dir defaults to ~/Pictures, and must exist prior to running gallery" << endl;
  std::exit(error ? 1 : 0);
}

void GalleryApplication::invalid_arg(QString arg) {
  QTextStream(stderr) << "Invalid argument '" << arg << "'" << endl;
  usage(true);
}

void GalleryApplication::process_args() {
  QStringList args = arguments();

  for (int i = 1; i < args.count(); ++i) {
    QString arg = args[i];
    QString value = (i + 1 < args.count() ? args[i + 1] : "");

    if (arg == "--help" || arg == "-h") {
      usage();
    } else if (arg == "--landscape") {
      is_portrait_ = false;
    } else if (arg == "--portrait") {
      is_portrait_ = true;
    } else if (arg == "--fullscreen") {
      is_fullscreen_ = true;
    } else if (arg == "--startup-timer") {
      startup_timer_ = true;
    } else if (arg == "--log-image-loading") {
      log_image_loading_ = true;
    } else {
      QString form_factor = arg.mid(2); // minus initial "--"

      if (arg.startsWith("--") && form_factors_.keys().contains(form_factor)) {
        form_factor_ = form_factor;
      } else if (arg.startsWith("--desktop_file_hint")) {
        // ignore this command line switch, hybris uses it to get application info
      } else if (i == args.count() - 1 && QDir(arg).exists()) {
        pictures_dir_ = QDir(arg);
      } else {
        invalid_arg(arg);
      }
    }
  }
}

void GalleryApplication::init_common() {
  // These need to be initialized before create_view() or init_collections().
  Resource::Init(applicationDirPath(), INSTALL_PREFIX);
  GalleryStandardImageProvider::Init();
  GalleryThumbnailImageProvider::Init();
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
    GalleryStandardImageProvider::instance());
  view_.engine()->addImageProvider(GalleryThumbnailImageProvider::PROVIDER_ID,
    GalleryThumbnailImageProvider::instance());
  view_.setSource(Resource::instance()->get_rc_url("qml/GalleryApplication.qml"));
  QObject::connect(view_.engine(), SIGNAL(quit()), this, SLOT(quit()));

  // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
  QObject* rootObject = dynamic_cast<QObject*>(view_.rootObject());
  QObject::connect(this, SIGNAL(media_loaded()), rootObject, SLOT(onLoaded()));

  if (is_fullscreen_)
    view_.showFullScreen();
  else
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
  
  // start the file monitor so that the collection contents will be updated as
  // new files arrive
  monitor_ = new MediaMonitor(pictures_dir_.path());
  QObject::connect(monitor_, SIGNAL(media_item_added(QFileInfo)), this,
    SLOT(on_media_item_added(QFileInfo)));  
  
  if (startup_timer_)
    qDebug() << "Startup took" << timer_.elapsed() << "milliseconds";
}

void GalleryApplication::start_init_collections() {
  init_collections();
}

GalleryApplication* GalleryApplication::instance() {
  return instance_;
}

void GalleryApplication::setObjectOwnership(QObject* object, QQmlEngine::ObjectOwnership ownership) {
  view_.engine()->setObjectOwnership(object, ownership);
}

void GalleryApplication::on_media_item_added(QFileInfo item_info) {
  Photo* new_photo = Photo::Fetch(item_info);
  
  if (new_photo)
    MediaCollection::instance()->Add(new_photo);
}

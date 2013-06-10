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

#include <QQuickItem>
#include <QString>
#include <QUrl>

#include "gallery-application.h"
#include "gallery-manager.h"

// album
#include "album.h"
#include "album-page.h"

// event
#include "event.h"

// media
#include "media-collection.h"
#include "media-monitor.h"
#include "media-source.h"

// photo
#include "photo.h"

// qml
#include "gallery-standard-image-provider.h"
#include "gallery-thumbnail-image-provider.h"
#include "qml-album-collection-model.h"
#include "qml-event-collection-model.h"
#include "qml-event-overview-model.h"
#include "qml-media-collection-model.h"

// util
#include "command-line-parser.h"
#include "resource.h"
#include "sharefile.h"

/*!
 * \brief GalleryApplication::GalleryApplication
 * \param argc
 * \param argv
 */
GalleryApplication::GalleryApplication(int& argc, char** argv)
    : QApplication(argc, argv),
      view_(),
      monitor_(NULL)
{
    bgu_size_ = QProcessEnvironment::systemEnvironment().value("GRID_UNIT_PX", "8").toInt();
    if (bgu_size_ <= 0)
        bgu_size_ = 8;

    timer_.start();
    form_factors_.insert("desktop", QSize(120, 80)); // In BGU.
    form_factors_.insert("tablet", QSize(160, 100));
    form_factors_.insert("phone", QSize(71, 40));
    form_factors_.insert("sidebar", QSize(71, 40));

    cmd_line_parser_ = new CommandLineParser(form_factors_);
    bool ok = cmd_line_parser_->processArguments(arguments());
    if (!ok)
        QApplication::quit();

    register_qml();

    GalleryManager::instance(cmd_line_parser()->picturesDir(), &view_,
                             cmd_line_parser()->logImageLoading());
}

/*!
 * \brief GalleryApplication::~GalleryApplication
 */
GalleryApplication::~GalleryApplication()
{
    delete monitor_;
}

/*!
 * \brief GalleryApplication::exec
 * \return
 */
int GalleryApplication::exec()
{
    create_view();

    // Delay init_collections() so the main loop is running before it kicks off.
    QTimer::singleShot(0, this, SLOT(start_init_collections()));

    return QApplication::exec();
}

/*!
 * \brief GalleryApplication::run_command is used for content sharing.
 * \param cmd
 * \param arg
 * \return
 */
bool GalleryApplication::run_command(const QString &cmd, const QString &arg)
{
    return QProcess::startDetached(cmd, QStringList(arg));
}

/*!
 * \brief GalleryApplication::register_qml
 */
void GalleryApplication::register_qml()
{
    qmlRegisterType<Album>("Gallery", 1, 0, "Album");
    qmlRegisterType<AlbumPage>("Gallery", 1, 0, "AlbumPage");
    qmlRegisterType<Event>("Gallery", 1, 0, "Event");
    qmlRegisterType<MediaSource>("Gallery", 1, 0, "MediaSource");
    qmlRegisterType<QmlAlbumCollectionModel>("Gallery", 1, 0, "AlbumCollectionModel");
    qmlRegisterType<QmlEventCollectionModel>("Gallery", 1, 0, "EventCollectionModel");
    qmlRegisterType<QmlEventOverviewModel>("Gallery", 1, 0, "EventOverviewModel");
    qmlRegisterType<QmlMediaCollectionModel>("Gallery", 1, 0, "MediaCollectionModel");
    qmlRegisterType<ShareFile>("Gallery", 1, 0, "ShareFile");
}

/*!
 * \brief GalleryApplication::create_view
 * Create the master QDeclarativeView that all the pages will operate within
 */
void GalleryApplication::create_view()
{
    view_.setTitle("Gallery");

    QSize size = form_factors_[cmd_line_parser()->formFactor()];

    if (cmd_line_parser_->isPortrait())
        size.transpose();

    view_.setResizeMode(QQuickView::SizeRootObjectToView);
    if (cmd_line_parser()->formFactor() == "desktop") {
        view_.setMinimumSize(QSize(60 * bgu_size_, 60 * bgu_size_));
    }

    QQmlContext *rootContext = view_.engine()->rootContext();
    rootContext->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
    rootContext->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
    rootContext->setContextProperty("FORM_FACTOR", QVariant(cmd_line_parser()->formFactor()));
    rootContext->setContextProperty("MAX_GL_TEXTURE_SIZE",
                                    QVariant(GalleryManager::instance()->resource()->maxTextureSize()));

    // Set ourselves up to expose functionality to run external commands from QML...
    view_.engine()->rootContext()->setContextProperty("APP", this);

    view_.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
                                     GalleryManager::instance()->gallery_standard_image_provider());
    view_.engine()->addImageProvider(GalleryThumbnailImageProvider::PROVIDER_ID,
                                     GalleryManager::instance()->gallery_thumbnail_image_provider());

    view_.setSource(Resource::getRcUrl("qml/GalleryApplication.qml"));
    QObject::connect(view_.engine(), SIGNAL(quit()), this, SLOT(quit()));

    // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
    QObject* rootObject = dynamic_cast<QObject*>(view_.rootObject());
    QObject::connect(this, SIGNAL(media_loaded()), rootObject, SLOT(onLoaded()));

    if (cmd_line_parser()->isFullscreen())
        view_.showFullScreen();
    else
        view_.show();
}

/*!
 * \brief GalleryApplication::init_collections
 */
void GalleryApplication::init_collections()
{
    GalleryManager::instance()->post_init();

    emit media_loaded();

    // start the file monitor so that the collection contents will be updated as
    // new files arrive
    monitor_ = new MediaMonitor(cmd_line_parser()->picturesDir().path());
    QObject::connect(monitor_, SIGNAL(mediaItemAdded(QFileInfo)), this,
                     SLOT(on_media_item_added(QFileInfo)));

    if (cmd_line_parser()->startupTimer())
        qDebug() << "Startup took" << timer_.elapsed() << "milliseconds";
}

/*!
 * \brief GalleryApplication::start_init_collections
 */
void GalleryApplication::start_init_collections()
{
    init_collections();
}

/*!
 * \brief GalleryApplication::instance
 * \return
 */
GalleryApplication* GalleryApplication::instance()
{
    return static_cast<GalleryApplication*>(qApp);
}

/*!
 * \brief GalleryApplication::on_media_item_added
 * \param item_info
 */
void GalleryApplication::on_media_item_added(QFileInfo item_info)
{
    MediaSource* newMedia = Photo::fetch(item_info);

    if (newMedia)
        GalleryManager::instance()->media_collection()->add(newMedia);
}

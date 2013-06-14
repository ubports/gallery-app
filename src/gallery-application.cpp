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
      m_view()
{
    m_bguSize = QProcessEnvironment::systemEnvironment().value("GRID_UNIT_PX", "8").toInt();
    if (m_bguSize <= 0)
        m_bguSize = 8;

    m_timer.start();
    m_formFactors.insert("desktop", QSize(120, 80)); // In BGU.
    m_formFactors.insert("tablet", QSize(160, 100));
    m_formFactors.insert("phone", QSize(71, 40));
    m_formFactors.insert("sidebar", QSize(71, 40));

    m_cmdLineParser = new CommandLineParser(m_formFactors);
    bool ok = m_cmdLineParser->processArguments(arguments());
    if (!ok)
        QApplication::quit();

    registerQML();

    GalleryManager::instance(cmdLineParser()->picturesDir(), &m_view,
                             cmdLineParser()->logImageLoading());
}

/*!
 * \brief GalleryApplication::~GalleryApplication
 */
GalleryApplication::~GalleryApplication()
{
}

/*!
 * \brief GalleryApplication::exec
 * \return
 */
int GalleryApplication::exec()
{
    createView();

    // Delay init_collections() so the main loop is running before it kicks off.
    QTimer::singleShot(0, this, SLOT(startInitCollections()));

    return QApplication::exec();
}

/*!
 * \brief GalleryApplication::runCommand is used for content sharing.
 * \param cmd
 * \param arg
 * \return
 */
bool GalleryApplication::runCommand(const QString &cmd, const QString &arg)
{
    return QProcess::startDetached(cmd, QStringList(arg));
}

/*!
 * \brief GalleryApplication::registerQML
 */
void GalleryApplication::registerQML()
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
 * \brief GalleryApplication::createView
 * Create the master QDeclarativeView that all the pages will operate within
 */
void GalleryApplication::createView()
{
    m_view.setTitle("Gallery");

    QSize size = m_formFactors[cmdLineParser()->formFactor()];

    if (m_cmdLineParser->isPortrait())
        size.transpose();

    m_view.setResizeMode(QQuickView::SizeRootObjectToView);
    if (cmdLineParser()->formFactor() == "desktop") {
        m_view.setMinimumSize(QSize(60 * m_bguSize, 60 * m_bguSize));
    }

    QQmlContext *rootContext = m_view.engine()->rootContext();
    rootContext->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
    rootContext->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
    rootContext->setContextProperty("FORM_FACTOR", QVariant(cmdLineParser()->formFactor()));
    rootContext->setContextProperty("MAX_GL_TEXTURE_SIZE",
                                    QVariant(GalleryManager::instance()->resource()->maxTextureSize()));

    // Set ourselves up to expose functionality to run external commands from QML...
    m_view.engine()->rootContext()->setContextProperty("APP", this);

    m_view.engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
                                     GalleryManager::instance()->galleryStandardImageProvider());
    m_view.engine()->addImageProvider(GalleryThumbnailImageProvider::PROVIDER_ID,
                                     GalleryManager::instance()->galleryThumbnailImageProvider());

    m_view.setSource(Resource::getRcUrl("qml/GalleryApplication.qml"));
    QObject::connect(m_view.engine(), SIGNAL(quit()), this, SLOT(quit()));

    // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
    QObject* rootObject = dynamic_cast<QObject*>(m_view.rootObject());
    QObject::connect(this, SIGNAL(mediaLoaded()), rootObject, SLOT(onLoaded()));

    if (cmdLineParser()->isFullscreen())
        m_view.showFullScreen();
    else
        m_view.show();
}

/*!
 * \brief GalleryApplication::initCollections
 */
void GalleryApplication::initCollections()
{
    GalleryManager::instance()->postInit();

    emit mediaLoaded();

    if (cmdLineParser()->startupTimer())
        qDebug() << "Startup took" << m_timer.elapsed() << "milliseconds";
}

/*!
 * \brief GalleryApplication::startInitCollections
 */
void GalleryApplication::startInitCollections()
{
    initCollections();
}

/*!
 * \brief GalleryApplication::instance
 * \return
 */
GalleryApplication* GalleryApplication::instance()
{
    return static_cast<GalleryApplication*>(qApp);
}

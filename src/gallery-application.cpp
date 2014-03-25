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

#include "gallery-application.h"
#include "content-communicator.h"
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
#include "qml-album-collection-model.h"
#include "qml-event-collection-model.h"
#include "qml-event-overview-model.h"
#include "qml-media-collection-model.h"

// util
#include "command-line-parser.h"
#include "resource.h"

#include <QQuickItem>
#include <QQuickView>
#include <QString>
#include <QTimer>
#include <QUrl>

QElapsedTimer* GalleryApplication::m_timer = 0;

/*!
 * \brief GalleryApplication::GalleryApplication
 * \param argc
 * \param argv
 */
GalleryApplication::GalleryApplication(int& argc, char** argv)
    : QApplication(argc, argv),
      m_view(new QQuickView()),
      m_contentCommunicator(new ContentCommunicator(this)),
      m_pickModeEnabled(false),
      m_defaultUiMode(BrowseContentMode)
{
    m_bguSize = QProcessEnvironment::systemEnvironment().value("GRID_UNIT_PX", "8").toInt();
    if (m_bguSize <= 0)
        m_bguSize = 8;

    m_formFactors.insert("desktop", QSize(120, 80)); // In BGU.
    m_formFactors.insert("tablet", QSize(160, 100));
    m_formFactors.insert("phone", QSize(71, 40));
    m_formFactors.insert("sidebar", QSize(71, 40));

    m_cmdLineParser = new CommandLineParser(m_formFactors);
    bool ok = m_cmdLineParser->processArguments(arguments());
    if (!ok)
        QApplication::quit();

    registerQML();

    m_galleryManager = new GalleryManager(isDesktopMode(), m_cmdLineParser->picturesDir(), m_view);
    m_galleryManager->logImageLoading(m_cmdLineParser->logImageLoading());
    if (m_cmdLineParser->pickModeEnabled())
        setDefaultUiMode(GalleryApplication::PickContentMode);

    QObject::connect(m_contentCommunicator, SIGNAL(photoRequested()),
                     this, SLOT(switchToPickMode()));

    if (m_cmdLineParser->startupTimer())
        qDebug() << "Construct GalleryApplication" << m_timer->elapsed() << "ms";
}

/*!
 * \brief GalleryApplication::~GalleryApplication
 */
GalleryApplication::~GalleryApplication()
{
    delete m_view;
    delete m_cmdLineParser;
    delete m_galleryManager;
}

/*!
 * \brief GalleryApplication::exec
 * \return
 */
int GalleryApplication::exec()
{
    createView();

    // Delay init_collections() so the main loop is running before it kicks off.
    QTimer::singleShot(0, this, SLOT(initCollections()));

    return QApplication::exec();
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

    qRegisterMetaType<QList<MediaSource*> >("MediaSourceList");
}

/*!
 * \brief GalleryApplication::isDesktopMode
 * Returns true if the DESKTOP_MODE env var is set
 */
bool GalleryApplication::isDesktopMode() const
{
    return(qEnvironmentVariableIsSet("DESKTOP_MODE") && (qgetenv("DESKTOP_MODE") == "1"));
}

/*!
 * \brief GalleryApplication::isFullScreen
 * Returns true if window is on FullScreen mode
 */
bool GalleryApplication::isFullScreen() const
{
    return m_view->windowState() == Qt::WindowFullScreen;
}

/*!
 * \brief GalleryApplication::createView
 * Create the master QDeclarativeView that all the pages will operate within
 */
void GalleryApplication::createView()
{
    m_view->setTitle("Gallery");

    QSize size = m_formFactors[m_cmdLineParser->formFactor()];

    if (m_cmdLineParser->isPortrait())
        size.transpose();

    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    if (isDesktopMode()) {
        m_view->setMinimumSize(QSize(60 * m_bguSize, 60 * m_bguSize));
    }

    QQmlContext *rootContext = m_view->engine()->rootContext();
    rootContext->setContextProperty("MANAGER", m_galleryManager);
    rootContext->setContextProperty("PICKER_HUB", m_contentCommunicator);
    rootContext->setContextProperty("DEVICE_WIDTH", QVariant(size.width()));
    rootContext->setContextProperty("DEVICE_HEIGHT", QVariant(size.height()));
    rootContext->setContextProperty("FORM_FACTOR", QVariant(m_cmdLineParser->formFactor()));
    rootContext->setContextProperty("MAX_GL_TEXTURE_SIZE",
                                    QVariant(m_galleryManager->resource()->maxTextureSize()));

    // Set ourselves up to expose functionality to run external commands from QML...
    m_view->engine()->rootContext()->setContextProperty("APP", this);

    m_view->setSource(Resource::getRcUrl("qml/GalleryApplication.qml"));
    QObject::connect(m_view->engine(), SIGNAL(quit()), this, SLOT(quit()));

    // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
    QObject* rootObject = dynamic_cast<QObject*>(m_view->rootObject());
    QObject::connect(this, SIGNAL(mediaLoaded()), rootObject, SLOT(onLoaded()));

    //run fullscreen if specified at command line or not in DESKTOP_MODE (i.e. on a device)
    m_view->show();

    if (m_cmdLineParser->isFullscreen() || !isDesktopMode()) {
        setFullScreen(true);
    }

    if (m_cmdLineParser->startupTimer())
        qDebug() << "GalleryApplication view created" << m_timer->elapsed() << "ms";
}

/*!
 * \brief GalleryApplication::initCollections
 */
void GalleryApplication::initCollections()
{
    QApplication::processEvents();

    m_galleryManager->postInit();
    m_view->engine()->addImageProvider(GalleryStandardImageProvider::PROVIDER_ID,
                                       m_galleryManager->takeGalleryStandardImageProvider());

    QApplication::processEvents();
    if (m_cmdLineParser->startupTimer())
        qDebug() << "GalleryManager initialized" << m_timer->elapsed() << "ms";

    emit mediaLoaded();
    if (m_cmdLineParser->startupTimer()) {
        qDebug() << "MainView loaded" << m_timer->elapsed() << "ms";
        qDebug() << "Startup took" << m_timer->elapsed() << "ms";
    }

    delete m_timer;
    m_timer = 0;
}

/*!
 * \brief GalleryApplication::setDefaultUiMode set the default UI mode. This might
 * get overridden during the lifetime
 * \param mode
 */
void GalleryApplication::setDefaultUiMode(GalleryApplication::UiMode mode)
{
    m_defaultUiMode = mode;
    setUiMode(mode);
}

/*!
 * \brief GalleryApplication::setUiMode set's the current UI mode
 * \param mode
 */
void GalleryApplication::setUiMode(GalleryApplication::UiMode mode)
{
    bool enablePickMode = (mode == PickContentMode);

    if (enablePickMode != m_pickModeEnabled) {
        m_pickModeEnabled = enablePickMode;
        Q_EMIT pickModeEnabledChanged();
    }
}

/*!
 * \brief GalleryApplication::pickModeEnabled returns true if the current UI
 * mode should be for picking acontent
 * \return
 */
bool GalleryApplication::pickModeEnabled() const
{
    return m_pickModeEnabled;
}

/*!
 * \brief GalleryApplication::switchToPickMode
 */
void GalleryApplication::switchToPickMode()
{
    setUiMode(PickContentMode);
}

/*!
 * \brief GalleryApplication::setFullScreen
 * Change window state to fullScreen or no state
 */
void GalleryApplication::setFullScreen(bool fullScreen)
{
    if(fullScreen) {
        m_view->setWindowState(Qt::WindowFullScreen);
    } else {
        m_view->setWindowState(Qt::WindowNoState);
    }

    Q_EMIT fullScreenChanged();
}

/*!
 * \brief GalleryApplication::returnPickedContent passes the selcted items to the
 * content manager
 * \param variant
 */
void GalleryApplication::returnPickedContent(QVariant variant)
{
    if (!variant.canConvert<QList<MediaSource*> >()) {
        qWarning() << Q_FUNC_INFO << variant << "is not a QList<MediaSource*>";
        return;
    }

    QList<MediaSource*> sources = qvariant_cast<QList<MediaSource*> >(variant);
    QVector<QUrl> selectedMedias;
    selectedMedias.reserve(sources.size());
    foreach (const MediaSource *media, sources) {
        selectedMedias.append(media->path());
    }
    m_contentCommunicator->returnPhotos(selectedMedias);

    if (m_defaultUiMode == BrowseContentMode) {
        setUiMode(BrowseContentMode);
    } else {
        // give the app and content-hub some time to finish taks (run the event loop)
        QTimer::singleShot(10, this, SLOT(quit()));
    }
}

/*!
 * \brief GalleryApplication::contentPickingCanceled tell the content manager, that
 * the picking was canceled
 */
void GalleryApplication::contentPickingCanceled()
{
    m_contentCommunicator->cancelTransfer();

    if (m_defaultUiMode == BrowseContentMode) {
        setUiMode(BrowseContentMode);
    } else {
        // give the app and content-hub some time to finish taks (run the event loop)
        QTimer::singleShot(10, this, SLOT(quit()));
    }
}

/*!
 * \brief GalleryApplication::startStartupTimer
 */
void GalleryApplication::startStartupTimer()
{
    if (!m_timer)
        m_timer = new QElapsedTimer();

    m_timer->restart();
}

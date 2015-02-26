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

// photoeditor
#include "photo-data.h"
#include "file-utils.h"
#include "photo-image-provider.h"

// qml
#include "qml-album-collection-model.h"
#include "qml-event-collection-model.h"
#include "qml-event-overview-model.h"
#include "qml-media-collection-model.h"

// util
#include "command-line-parser.h"
#include "urlhandler.h"
#include "resource.h"

#include <QQuickItem>
#include <QQuickView>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QtGui/QGuiApplication>

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
      m_defaultUiMode(BrowseContentMode),
      m_mediaTypeFilter(MediaSource::None),
      m_mediaFile(""),
      m_mediaLoadedTimer(this),
      m_mediaLoaded(false)
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

    if (qgetenv("QT_LOAD_TESTABILITY") == "1") {
        QLibrary testLib(QLatin1String("qttestability"));
        if (testLib.load()) {
            typedef void (*TasInitialize)(void);
            TasInitialize initFunction = (TasInitialize)testLib.resolve("qt_testability_init");
            if (initFunction) {
                initFunction();
            } else {
                qCritical("Library qttestability resolve failed!");
            }
        } else {
            qCritical("Library qttestability load failed!");
        }
    }

    m_urlHandler = new UrlHandler();

    registerQML();

    m_galleryManager = new GalleryManager(isDesktopMode(), m_cmdLineParser->picturesDir());
    if (m_cmdLineParser->pickModeEnabled())
        setDefaultUiMode(GalleryApplication::PickContentMode);

    QObject::connect(m_galleryManager, SIGNAL(consistencyCheckFinished()),
                     this, SLOT(consistencyCheckFinished()));

    QObject::connect(m_galleryManager, SIGNAL(collectionChanged()),
                     this, SLOT(onCollectionChanged()));

    // Used to hide the Loading Screen after a time out
    QObject::connect(m_galleryManager, SIGNAL(consistencyCheckFinished()),
                     this, SLOT(onCollectionChanged()));

    QObject::connect(m_contentCommunicator, SIGNAL(mediaRequested(QString)),
                     this, SLOT(switchToPickMode(QString)));

    QObject::connect(m_contentCommunicator, SIGNAL(mediaImported()),
                     this, SLOT(switchToEventsView()));

    if (m_cmdLineParser->startupTimer())
        qDebug() << "Construct GalleryApplication" << m_timer->elapsed() << "ms";

    m_mediaLoadedTimer.setSingleShot(false);
    m_mediaLoadedTimer.setInterval(100);
    QObject::connect(&m_mediaLoadedTimer, SIGNAL(timeout()), this, SLOT(onMediaLoaded()));
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
    qmlRegisterType<PhotoData>("Gallery", 1, 0, "GalleryPhotoData");
    qmlRegisterSingletonType<FileUtils>("Gallery", 1, 0, "GalleryFileUtils", exportFileUtilsSingleton);  

    qRegisterMetaType<QList<MediaSource*> >("MediaSourceList");
    qRegisterMetaType<QSet<DataObject*> >("QSet<DataObject*>");
}

/*!
 * \brief GalleryApplication::isDesktopMode
 * Returns true if the DESKTOP_MODE env var is set
 */
bool GalleryApplication::isDesktopMode() const
{

  // Assume that platformName (QtUbuntu) with ubuntu
  // in name means it's running on device
  // TODO: replace this check with SDK call for formfactor
  QString platform = QGuiApplication::platformName();
  return !((platform == "ubuntu") || (platform == "ubuntumirclient"));

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
 * \brief GalleryApplication::getMediaFile
 * Returns the media file passed as a parameter
 */
const QString& GalleryApplication::getMediaFile() const
{
    return m_mediaFile;
}

/*!
 * \brief GalleryApplication::createView
 * Create the master QDeclarativeView that all the pages will operate within
 */
void GalleryApplication::createView()
{
    m_view->setTitle("Gallery");

    PhotoImageProvider* provider = new PhotoImageProvider();
    provider->setLogging(true);
    m_view->engine()->addImageProvider(PhotoImageProvider::PROVIDER_ID,
                             provider);

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

    // Set ourselves up to expose functionality to run external commands from QML...
    m_view->engine()->rootContext()->setContextProperty("APP", this);

    m_view->setSource(Resource::getRcUrl("qml/GalleryApplication.qml"));
    QObject::connect(m_view->engine(), SIGNAL(quit()), this, SLOT(quit()));

    // Hook up our media_loaded signal to GalleryApplication's onLoaded function.
    QObject* rootObject = dynamic_cast<QObject*>(m_view->rootObject());
    QObject::connect(this, SIGNAL(mediaLoaded()), rootObject, SLOT(onLoaded()));

    //run fullscreen if specified at command line
    if (m_cmdLineParser->isFullscreen()) {
        setFullScreen(true);
        m_view->showFullScreen();
    } else {
        m_view->show();
    }

    if (m_cmdLineParser->startupTimer())
        qDebug() << "GalleryApplication view created" << m_timer->elapsed() << "ms";

    setMediaFile(m_cmdLineParser->mediaFile());
}

QObject* GalleryApplication::exportFileUtilsSingleton(QQmlEngine *engine,
                                              QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new FileUtils();
}

/*!
 * \brief GalleryApplication::initCollections
 */
void GalleryApplication::initCollections()
{
    QApplication::processEvents();

    m_galleryManager->postInit();

    QApplication::processEvents();
    if (m_cmdLineParser->startupTimer())
        qDebug() << "GalleryManager initialized" << m_timer->elapsed() << "ms";
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
 * \brief GalleryApplication::contentTypeFilter returns the type of
 * content to display in the UI. If the empty string is returned then
 * no content filter is in place.
 * \return
 */
MediaSource::MediaType GalleryApplication::mediaTypeFilter() const
{
    return m_mediaTypeFilter;
}

/*!
 * \brief GalleryApplication::switchToPickMode
 * \param QString the type of media to pick or blank string for any type
 */
void GalleryApplication::switchToPickMode(QString mediaTypeFilter)
{
    setUiMode(PickContentMode);

    MediaSource::MediaType newFilter;
    if (mediaTypeFilter == "pictures") newFilter = MediaSource::Photo;
    else if (mediaTypeFilter == "videos") newFilter = MediaSource::Video;
    else newFilter = MediaSource::None;

    if (newFilter != m_mediaTypeFilter) {
        m_mediaTypeFilter = newFilter;
        Q_EMIT mediaTypeFilterChanged();
    }
}

/*!
 * \brief GalleryApplication::switchToEventsView
 */
void GalleryApplication::switchToEventsView()
{
    Q_EMIT eventsViewRequested();
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

void GalleryApplication::setMediaFile(const QString &mediaFile)
{
    if(!mediaFile.isEmpty()) {
        m_mediaFile = "file://" + mediaFile;
        m_mediaLoadedTimer.start();
    }
}

void GalleryApplication::onMediaLoaded()
{
    if (m_mediaLoaded) {
        m_mediaLoadedTimer.stop();
        Q_EMIT mediaFileChanged();
    }
}

void GalleryApplication::onCollectionChanged()
{
    if (!m_mediaLoaded) {
        emit mediaLoaded();
        m_mediaLoaded = true;

        if (m_cmdLineParser->startupTimer()) {
            qDebug() << "MainView loaded" << m_timer->elapsed() << "ms";
            qDebug() << "Startup took" << m_timer->elapsed() << "ms";
        }

        delete m_timer;
        m_timer = 0;
    }
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

/*!
 * \brief GalleryApplication::consistencyCheckFinished triggered when the media
 * monitor finishes its consistency check
 */
void GalleryApplication::consistencyCheckFinished()
{
    // Register content hub integration after media monitor has finished
    // its consistency check, as new images may be added by the import handler
    // during start-up.
    m_contentCommunicator->registerWithHub();
}

void GalleryApplication::parseUri(const QString &arg)
{
    if (m_urlHandler->processUri(arg)) {
        setMediaFile(m_urlHandler->mediaFile());
    }
}

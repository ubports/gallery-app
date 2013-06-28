/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *  Nicolas d'Offay <nicolas.doffay@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gallery-manager.h"
#include "media-object-factory.h"

// album
#include "album-collection.h"
#include "album-default-template.h"

// database
#include "database.h"
#include "media-table.h"

// event
#include "event-collection.h"

// media
#include "media-collection.h"
#include "media-monitor.h"
#include "preview-manager.h"

// qml
#include "gallery-standard-image-provider.h"
#include "gallery-thumbnail-image-provider.h"
#include "qml-media-collection-model.h"

// util
#include "resource.h"

#include <QElapsedTimer>

#include <exiv2/exiv2.hpp>

GalleryManager* GalleryManager::m_galleryManager = NULL;

/*!
 * \brief GalleryManager::GalleryManager
 * \param picturesDir
 * \param view
 * \param logImageLoading
 */
GalleryManager::GalleryManager(const QString& picturesDir,
                               QQuickView *view)
    : collectionsInitialised(false),
      m_resource(new Resource(picturesDir, view)),
      m_standardImageProvider(new GalleryStandardImageProvider()),
      m_thumbnailImageProvider(new GalleryThumbnailImageProvider()),
      m_database(0),
      m_defaultTemplate(0),
      m_mediaCollection(0),
      m_albumCollection(0),
      m_eventCollection(0),
      m_previewManager(0),
      m_monitor(0)
{
    const int maxTextureSize = m_resource->maxTextureSize();
    m_standardImageProvider->setMaxLoadResolution(maxTextureSize);
    m_mediaFactory = new MediaObjectFactory();

    m_galleryManager = this;
}

/*!
 * \brief GalleryManager::~GalleryManager
 */
GalleryManager::~GalleryManager()
{
    delete m_mediaLibrary;
    m_mediaLibrary = 0;

    delete m_monitor;
    m_monitor = 0;

    delete m_resource;
    m_resource = 0;

    delete m_standardImageProvider;
    m_standardImageProvider = 0;

    delete m_thumbnailImageProvider;
    m_thumbnailImageProvider = 0;

    delete m_mediaFactory;
    m_mediaFactory = 0;

    delete m_database;
    m_database = 0;

    delete m_defaultTemplate;
    m_defaultTemplate = 0;

    delete m_mediaCollection;
    m_mediaCollection = 0;

    delete m_albumCollection;
    m_albumCollection = 0;

    delete m_eventCollection;
    m_eventCollection = 0;

    delete m_previewManager;
    m_previewManager = 0;
}

/*!
 * \brief GalleryManager::instance
 * \param application_path_dir the directory of where the executable is
 * \param picturesDir the directory of the images
 * \param view the view is used to determine the max texture size
 * \param logImageLoading if true, the image loadings times are printed to stdout
 * \return
 */
GalleryManager* GalleryManager::instance()
{
    Q_ASSERT(m_galleryManager);
    return m_galleryManager;
}

/*!
 * \brief GalleryManager::returnPickedContent passes the selcted items to the
 * content manager
 * \param variant
 */
void GalleryManager::returnPickedContent(QVariant variant)
{
    if (!variant.canConvert<QList<MediaSource*> >()) {
        qWarning() << Q_FUNC_INFO << variant << "is not a QList<MediaSource*>";
        return;
    }

    QList<MediaSource*> sources = qvariant_cast<QList<MediaSource*> >(variant);

    foreach (const MediaSource *media, sources) {
        //FIXME call content manager API
        qDebug() << "Picked media:" << media->path();
    }
}

/*!
 * \brief GalleryManager::contentPickingCanceled tell the content manager, that
 * the picking was canceled
 */
void GalleryManager::contentPickingCanceled()
{
    //FIXME call content manager API
}

/*!
 * \brief GalleryManager::enableContentLoadFilter enable filter to load only
 * content of certain type
 * \param filterType
 */
void GalleryManager::enableContentLoadFilter(MediaSource::MediaType filterType)
{
    m_mediaFactory->enableContentLoadFilter(filterType);
}

/*!
 * \brief GalleryManager::postInit
 * Called after main loop is initialised. See GalleryApplication::exec() comments.
 */
void GalleryManager::postInit()
{
    Q_ASSERT(m_resource);

    if (!collectionsInitialised)
    {
        QElapsedTimer t;
        t.start();
        qDebug() << "Opening" << m_resource->mediaDirectories() << "...";

        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);

        m_database = new Database(m_resource->databaseDirectory(),
                                 m_resource->getRcUrl("sql").path());
        m_mediaFactory->setMediaTable(m_database->getMediaTable());
        m_database->getMediaTable()->verifyFiles();
        m_defaultTemplate = new AlbumDefaultTemplate();
        m_mediaCollection = new MediaCollection();

        initPreviewManager();
        startFileMonitoring();
        fillMediaCollection();

        collectionsInitialised = true;

        qDebug() << "Opened" << m_resource->mediaDirectories()
                 << m_mediaCollection->count() << "media files loaded in"
                 << t.elapsed() << "ms -"
                 <<  (qreal)t.elapsed() / (qreal)m_mediaCollection->count() << "ms per media";
    }
}

/*!
 * \brief GalleryManager::albumCollection returns the collection of all albums
 * \return
 */
AlbumCollection *GalleryManager::albumCollection()
{
    if (!m_albumCollection)
        m_albumCollection = new AlbumCollection();

    return m_albumCollection;
}

/*!
 * \brief GalleryManager::eventCollection returns the collection of all events
 * \return
 */
EventCollection *GalleryManager::eventCollection()
{
    if (!m_eventCollection)
        m_eventCollection = new EventCollection;

    return m_eventCollection;
}

/*!
 * \brief GalleryManager::mediaLibrary return the collection of all medias
 * \return
 */
QmlMediaCollectionModel *GalleryManager::mediaLibrary() const
{
    if (m_mediaLibrary == 0) {
        m_mediaLibrary = new QmlMediaCollectionModel();
        m_mediaLibrary->setMonitored(true);
    }

    return m_mediaLibrary;
}

/*!
 * \brief GalleryManager::logImageLoading enabled or disbaled logging image load
 * times to stdout
 * \param log
 */
void GalleryManager::logImageLoading(bool log)
{
    m_standardImageProvider->setLogging(log);
    m_thumbnailImageProvider->setLogging(log);
}

/*!
 * \brief GalleryManager::initPreviewManager creates the PreviewManager,
 * assigns it to all needed objects and creates all signal slot connection.
 */
void GalleryManager::initPreviewManager()
{
    Q_ASSERT(m_resource);
    Q_ASSERT(m_mediaCollection);
    Q_ASSERT(m_standardImageProvider);
    Q_ASSERT(m_thumbnailImageProvider);

    if (m_previewManager)
        return;

    m_previewManager = new PreviewManager(m_resource->thumbnailDirectory(),
                                          m_mediaCollection);

    m_standardImageProvider->setPreviewManager(m_previewManager);
    m_thumbnailImageProvider->setPreviewManager(m_previewManager);

    // Monitor MediaCollection for all new MediaSources
    QObject::connect(m_mediaCollection,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)),
                     m_previewManager,
                     SLOT(onMediaAddedRemoved(const QSet<DataObject*>*,const QSet<DataObject*>*)));

    QObject::connect(m_mediaCollection,
                     SIGNAL(destroying(const QSet<DataObject*>*)),
                     m_previewManager,
                     SLOT(onMediaDestroying(const QSet<DataObject*>*)));

    // Verify previews for all existing added MediaSources
    m_previewManager->onMediaAddedRemoved(&m_mediaCollection->getAsSet(), NULL);
}

/*!
 * \brief GalleryManager::fillMediaCollection fills the MediaCollection with
 * the content of the picture directory
 */
void GalleryManager::fillMediaCollection()
{
    Q_ASSERT(m_mediaCollection);

    QSet<DataObject*> photos;
    foreach (const QString &dirName, m_resource->mediaDirectories()) {
        QDir mediaDir(dirName);
        mediaDir.setFilter(QDir::Files);
        mediaDir.setSorting(QDir::Name);

        const QStringList filenames = mediaDir.entryList();
        foreach (const QString& filename, filenames) {
            QFileInfo file(mediaDir, filename);
            DataObject *media = m_mediaFactory->create(file);
            if (media) {
                photos.insert(media);
            }
        }
    }

    m_mediaCollection->addMany(photos);
}

/*!
 * \brief GalleryManager::startFileMonitoring start the file monitor so that the
 * collection contents will be updated as new files arrive
 */
void GalleryManager::startFileMonitoring()
{
    if (m_monitor)
        return;

    m_monitor = new MediaMonitor();
    QObject::connect(m_monitor, SIGNAL(mediaItemAdded(QString)),
                     this, SLOT(onMediaItemAdded(QString)));
    m_monitor->startMonitoring(m_resource->mediaDirectories());
}

/*!
 * \brief GalleryApplication::onMediaItemAdded
 * \param file
 */
void GalleryManager::onMediaItemAdded(QString file)
{
    QFileInfo fi(file);
    MediaSource* media = m_mediaCollection->mediaFromFileinfo(fi);
    if (media == 0) {
        media = m_mediaFactory->create(fi);
    }
    if (media) {
        m_mediaCollection->add(media);
    }
}

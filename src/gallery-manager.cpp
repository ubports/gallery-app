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

// qml
#include "qml-media-collection-model.h"

// util
#include "resource.h"

#include <QApplication>
#include <QElapsedTimer>

#include <exiv2/exiv2.hpp>

GalleryManager* GalleryManager::m_galleryManager = NULL;

/*!
 * \brief GalleryManager::GalleryManager
 * \param picturesDir
 * \param logImageLoading
 */
GalleryManager::GalleryManager(bool desktopMode,
                               const QString& picturesDir)
    : collectionsInitialised(false),
      m_resource(new Resource(desktopMode, picturesDir)),
      m_database(0),
      m_defaultTemplate(0),
      m_mediaCollection(0),
      m_albumCollection(0),
      m_eventCollection(0),
      m_monitor(0),
      m_desktopMode(desktopMode),
      m_mediaLibrary(0)
{
    m_mediaFactory = new MediaObjectFactory(m_desktopMode, m_resource);

    QObject::connect(m_mediaFactory, SIGNAL(mediaObjectCreated(MediaSource*)),
                     this, SLOT(onMediaObjectCreated(MediaSource*)));
    QObject::connect(m_mediaFactory, SIGNAL(mediaFromDBLoaded(QSet<DataObject *>)),
                     this, SLOT(onMediaFromDBLoaded(QSet<DataObject *>)));


    m_galleryManager = this;
}

/*!
 * \brief GalleryManager::~GalleryManager
 */
GalleryManager::~GalleryManager()
{
    delete m_monitor;
    delete m_mediaFactory;
    delete m_mediaLibrary;
    delete m_albumCollection;
    delete m_eventCollection;
    delete m_database;
    delete m_defaultTemplate;
    delete m_resource;
    delete m_mediaCollection;
}

/*!
 * \brief GalleryManager::instance
 * \param application_path_dir the directory of where the executable is
 * \param picturesDir the directory of the images
 * \param logImageLoading if true, the image loadings times are printed to stdout
 * \return
 */
GalleryManager* GalleryManager::instance()
{
    Q_ASSERT(m_galleryManager);
    return m_galleryManager;
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
        m_defaultTemplate = new AlbumDefaultTemplate();
        m_mediaCollection = new MediaCollection(m_database->getMediaTable());

        QObject::connect(m_mediaCollection, SIGNAL(collectionChanged()),
                      this, SIGNAL(collectionChanged()));

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
    if (!m_albumCollection) {
        m_albumCollection = new AlbumCollection(m_mediaCollection,
                                                m_database->getAlbumTable(),
                                                m_defaultTemplate);
    }

    return m_albumCollection;
}

/*!
 * \brief GalleryManager::eventCollection returns the collection of all events
 * \return
 */
EventCollection *GalleryManager::eventCollection()
{
    if (!m_eventCollection) {
        m_eventCollection = new EventCollection();

        // Monitor MediaCollection to create/destroy Events, one for each day of
        // media found
        QObject::connect(
                    m_mediaCollection,
                    SIGNAL(contentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)),
                    m_eventCollection,
                    SLOT(onMediaAddedRemoved(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)));
        // seed what's already present
        m_eventCollection->onMediaAddedRemoved(&(m_mediaCollection->getAsSet()), NULL, true);
    }

    return m_eventCollection;
}

/*!
 * \brief GalleryManager::mediaLibrary return the collection of all medias
 * \return
 */
QmlMediaCollectionModel *GalleryManager::mediaLibrary() const
{
    if (m_mediaLibrary == 0) {
        GalleryManager *self = const_cast<GalleryManager*>(this);
        m_mediaLibrary = new QmlMediaCollectionModel(self);
        m_mediaLibrary->setMonitored(true);
    }

    return m_mediaLibrary;
}

/*!
 * \brief GalleryManager::fillMediaCollection fills the MediaCollection with
 * the content of the picture directory
 */
void GalleryManager::fillMediaCollection()
{
    Q_ASSERT(m_mediaCollection);
    m_mediaFactory->loadMediaFromDB();
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
    QObject::connect(m_mediaCollection, SIGNAL(mediaIsBusy(bool)),
                     m_monitor, SLOT(setMonitoringOnHold(bool)));
    QObject::connect(m_monitor, SIGNAL(mediaItemAdded(QString, int)),
                     this, SLOT(onMediaItemAdded(QString, int)));
    QObject::connect(m_monitor, SIGNAL(mediaItemRemoved(qint64)),
                     this, SLOT(onMediaItemRemoved(qint64)));
    QObject::connect(m_monitor, SIGNAL(consistencyCheckFinished()),
                     this, SIGNAL(consistencyCheckFinished()));

    m_monitor->startMonitoring(m_resource->mediaDirectories());
    m_monitor->checkConsistency(m_mediaCollection);
}

/*!
 * \brief GalleryApplication::onMediaItemAdded
 * \param file
 */
void GalleryManager::onMediaItemAdded(QString file, int priority)
{
    if (! m_mediaCollection->containsFile(file)) {
        QFileInfo fi(file);
        m_mediaFactory->create(fi, priority, m_desktopMode, m_resource);
    }
}

/*!
 * \brief GalleryManager::onMediaItemRemoved
 * \param mediaId
 */
void GalleryManager::onMediaItemRemoved(qint64 mediaId)
{
    m_mediaCollection->destroy(mediaId, false);
}

/*!
 * \brief GalleryManager::onMediaObjectCreated
 * \param mediaObject
 */
void GalleryManager::onMediaObjectCreated(MediaSource *mediaObject)
{
    m_mediaCollection->add(mediaObject);
}

/*!
 * \brief GalleryManager::onMediaFromDBLoaded
 * \param mediaFromDB
 */
void GalleryManager::onMediaFromDBLoaded(QSet<DataObject *> mediaFromDB)
{
    m_mediaCollection->addMany(mediaFromDB);
    m_mediaFactory->clear();

    startFileMonitoring();
}

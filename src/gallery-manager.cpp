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
#include "photo.h"
#include "preview-manager.h"

// qml
#include "gallery-standard-image-provider.h"
#include "gallery-thumbnail-image-provider.h"

// util
#include "resource.h"

#include <exiv2/exiv2.hpp>

GalleryManager* GalleryManager::m_galleryManager = NULL;

/*!
 * \brief GalleryManager::instance
 * \param application_path_dir the directory of where the executable is
 * \param picturesDir the directory of the images
 * \param view the view is used to determine the max texture size
 * \param logImageLoading if true, the image loadings times are printed to stdout
 * \return
 */
GalleryManager* GalleryManager::instance(const QDir &picturesDir,
                                         QQuickView *view, const bool logImageLoading)
{
    if (!m_galleryManager)
        m_galleryManager = new GalleryManager(picturesDir, view, logImageLoading);

    return m_galleryManager;
}

/*!
 * \brief GalleryManager::GalleryManager
 * \param picturesDir
 * \param view
 * \param logImageLoading
 */
GalleryManager::GalleryManager(const QDir& picturesDir,
                               QQuickView *view, const bool logImageLoading)
    : collectionsInitialised(false),
      m_resource(new Resource(picturesDir.path(), view)),
      m_standardImageProvider(new GalleryStandardImageProvider()),
      m_thumbnailImageProvider(new GalleryThumbnailImageProvider()),
      m_database(NULL),
      m_defaultTemplate(NULL),
      m_mediaCollection(NULL),
      m_albumCollection(NULL),
      m_eventCollection(NULL),
      m_previewManager(NULL)
{
    const int maxTextureSize = m_resource->maxTextureSize();
    m_standardImageProvider->setMaxLoadResolution(maxTextureSize);
    m_standardImageProvider->setLogging(logImageLoading);
    m_thumbnailImageProvider->setLogging(logImageLoading);
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
        qDebug() << "Opening" << m_resource->picturesDirectory() << "...";

        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);

        m_database = new Database(m_resource->databaseDirectory(),
                                 m_resource->getRcUrl("sql").path());
        m_database->getMediaTable()->verifyFiles();
        m_defaultTemplate = new AlbumDefaultTemplate();
        m_mediaCollection = new MediaCollection();
        fillMediaCollection();
        m_albumCollection = new AlbumCollection();
        m_eventCollection = new EventCollection();

        collectionsInitialised = true;

        initPreviewManager();

        qDebug() << "Opened" << m_resource->picturesDirectory();
    }
}

/*!
 * \brief GalleryManager::~GalleryManager
 */
GalleryManager::~GalleryManager()
{
    delete m_resource;
    m_resource = NULL;

    delete m_standardImageProvider;
    m_standardImageProvider = NULL;

    delete m_thumbnailImageProvider;
    m_thumbnailImageProvider = NULL;

    delete m_database;
    m_database = NULL;

    delete m_defaultTemplate;
    m_defaultTemplate = NULL;

    delete m_mediaCollection;
    m_mediaCollection = NULL;

    delete m_albumCollection;
    m_albumCollection = NULL;

    delete m_eventCollection;
    m_eventCollection = NULL;

    delete m_previewManager;
    m_previewManager = NULL;
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

    QDir mediaDir(m_resource->picturesDirectory());
    mediaDir.setFilter(QDir::Files);
    mediaDir.setSorting(QDir::Name);

    QSet<DataObject*> photos;
    const QStringList filenames = mediaDir.entryList();
    foreach (const QString& filename, filenames) {
        QFileInfo file(mediaDir, filename);
        Photo *p = Photo::load(file);
        if (!p)
            continue;

        photos.insert(p);
    }

    m_mediaCollection->addMany(photos);
}

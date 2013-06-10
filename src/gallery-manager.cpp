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

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

/*!
 * \brief GalleryManager::instance
 * \param application_path_dir the directory of where the executable is
 * \param pictures_dir the directory of the images
 * \param view the view is used to determine the max texture size
 * \param log_image_loading if true, the image loadings times are printed to stdout
 * \return
 */
GalleryManager* GalleryManager::instance(const QDir &pictures_dir,
                                         QQuickView *view, const bool log_image_loading)
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager(pictures_dir, view, log_image_loading);

    return gallery_mgr_;
}

GalleryManager::GalleryManager(const QDir& pictures_dir,
                               QQuickView *view, const bool log_image_loading)
    : collections_initialised(false),
      resource_(new Resource(pictures_dir.path(), view)),
      gallery_standard_image_provider_(new GalleryStandardImageProvider()),
      gallery_thumbnail_image_provider_(new GalleryThumbnailImageProvider()),
      database_(NULL),
      default_template_(NULL),
      media_collection_(NULL),
      album_collection_(NULL),
      event_collection_(NULL),
      preview_manager_(NULL)
{
    const int maxTextureSize = resource_->maxTextureSize();
    gallery_standard_image_provider_->setMaxLoadResolution(maxTextureSize);
    gallery_standard_image_provider_->setLogging(log_image_loading);
    gallery_thumbnail_image_provider_->setLogging(log_image_loading);
}

void GalleryManager::post_init()
{
    Q_ASSERT(resource_);

    if (!collections_initialised)
    {
        qDebug() << "Opening" << resource_->picturesDirectory() << "...";

        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);

        database_ = new Database(resource_->databaseDirectory(),
                                 resource_->getRcUrl("sql").path());
        database_->getMediaTable()->verifyFiles();
        default_template_ = new AlbumDefaultTemplate();
        media_collection_ = new MediaCollection();
        fillMediaCollection();
        album_collection_ = new AlbumCollection();
        event_collection_ = new EventCollection();

        collections_initialised = true;

        initPreviewManager();

        qDebug() << "Opened" << resource_->picturesDirectory();
    }
}

GalleryManager::~GalleryManager()
{
    delete resource_;
    resource_ = NULL;

    delete gallery_standard_image_provider_;
    gallery_standard_image_provider_ = NULL;

    delete gallery_thumbnail_image_provider_;
    gallery_thumbnail_image_provider_ = NULL;

    delete database_;
    database_ = NULL;

    delete default_template_;
    default_template_ = NULL;

    delete media_collection_;
    media_collection_ = NULL;

    delete album_collection_;
    album_collection_ = NULL;

    delete event_collection_;
    event_collection_ = NULL;

    delete preview_manager_;
    preview_manager_ = NULL;
}

/*!
 * \brief GalleryManager::initPreviewManager creates the PreviewManager,
 * assigns it to all needed objects and creates all signal slot connection.
 */
void GalleryManager::initPreviewManager()
{
    Q_ASSERT(resource_);
    Q_ASSERT(media_collection_);
    Q_ASSERT(gallery_standard_image_provider_);
    Q_ASSERT(gallery_thumbnail_image_provider_);

    if (preview_manager_)
        return;

    preview_manager_ = new PreviewManager(resource_->thumbnailDirectory(),
                                          media_collection_);

    gallery_standard_image_provider_->setPreviewManager(preview_manager_);
    gallery_thumbnail_image_provider_->setPreviewManager(preview_manager_);

    // Monitor MediaCollection for all new MediaSources
    QObject::connect(media_collection_,
                     SIGNAL(contentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)),
                     preview_manager_,
                     SLOT(onMediaAddedRemoved(const QSet<DataObject*>*,const QSet<DataObject*>*)));

    QObject::connect(media_collection_,
                     SIGNAL(destroying(const QSet<DataObject*>*)),
                     preview_manager_,
                     SLOT(onMediaDestroying(const QSet<DataObject*>*)));

    // Verify previews for all existing added MediaSources
    preview_manager_->onMediaAddedRemoved(&media_collection_->getAsSet(), NULL);
}

/*!
 * \brief GalleryManager::fillMediaCollection fills the MediaCollection with
 * the content of the picture directory
 */
void GalleryManager::fillMediaCollection()
{
    Q_ASSERT(media_collection_);

    QDir mediaDir(resource_->picturesDirectory());
    mediaDir.setFilter(QDir::Files);
    mediaDir.setSorting(QDir::Name);

    QSet<DataObject*> photos;
    const QStringList filenames = mediaDir.entryList();
    foreach (const QString& filename, filenames) {
        QFileInfo file(mediaDir, filename);
        Photo *p = Photo::Load(file);
        if (!p)
            continue;

        photos.insert(p);
    }

    media_collection_->addMany(photos);
}

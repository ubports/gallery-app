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
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "core/command-line-parser.h"
#include "database/database.h"
#include "event/event-collection.h"
#include "media/media-collection.h"
#include "media/preview-manager.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include "util/resource.h"

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

GalleryManager* GalleryManager::instance(const QString &application_path_dir, const QDir &pictures_dir, const bool log_image_loading)
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager(application_path_dir, pictures_dir, log_image_loading);

    return gallery_mgr_;
}

GalleryManager::GalleryManager(const QString& application_path_dir, const QDir& pictures_dir, const bool log_image_loading)
    : collections_initialised(false),
      resource_(new Resource(application_path_dir, INSTALL_PREFIX)),
      gallery_standard_image_provider_(new GalleryStandardImageProvider(log_image_loading)),
      gallery_thumbnail_image_provider_(new GalleryThumbnailImageProvider(log_image_loading)),
      database_(NULL),
      default_template_(NULL),
      media_collection_(NULL),
      album_collection_(NULL),
      event_collection_(NULL),
      preview_manager_(NULL),
      pictures_dir_(pictures_dir)
{
}

void GalleryManager::post_init()
{
    if (!collections_initialised)
    {
        qDebug("Opening %s...", qPrintable(pictures_dir_.path()));

        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);

        database_ = new Database(pictures_dir_);
        database_->get_media_table()->verify_files();
        default_template_ = new AlbumDefaultTemplate();
        media_collection_ = new MediaCollection(pictures_dir_);
        album_collection_ = new AlbumCollection();
        event_collection_ = new EventCollection();
        preview_manager_ = new PreviewManager();

        collections_initialised = true;

        qDebug("Opened %s", qPrintable(pictures_dir_.path()));
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

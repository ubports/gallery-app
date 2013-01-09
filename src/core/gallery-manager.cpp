/*
 * Copyright (C) 2012 Canonical, Ltd.
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
 * gallerymanager.cpp4
 *
 * Simple class which encapsulates instantiates objects which require only one instance.
 *
 */

#include "gallery-manager.h"
#include "photo/photo-metadata.h"
#include "database/database.h"
#include "gallery-application.h"
#include "util/resource.h"
#include "event/event-collection.h"
#include "album/album-collection.h"
#include "media/media-collection.h"
#include "album/album-default-template.h"
#include "media/preview-manager.h"
#include "album/album-default-template.h"
#include "media/media-collection.h"
#include "qml/gallery-standard-image-provider.h"
#include "qml/gallery-thumbnail-image-provider.h"
#include <QDir>

GalleryManager* GalleryManager::gallery_mgr_ = NULL;

GalleryManager* GalleryManager::GetInstance()
{
    if (!gallery_mgr_)
        gallery_mgr_ = new GalleryManager();

    return gallery_mgr_;
}

GalleryManager::GalleryManager() : collections_initialised(false)
{
    resource_ = new Resource(GalleryApplication::instance()->applicationDirPath(), INSTALL_PREFIX);
    gallery_standard_image_provider_ = new GalleryStandardImageProvider();
    gallery_thumbnail_image_provider = new GalleryThumbnailImageProvider();
}

void GalleryManager::PostInit()
{
    if (!collections_initialised)
    {
        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);
        //photo_meta_data_ = new PhotoMetadata();

        database_ = new Database(GalleryApplication::instance()->GetPicturesDir(), GalleryApplication::instance());
        database_->get_media_table()->verify_files();
        default_template_ = new AlbumDefaultTemplate();
        media_collection_ = new MediaCollection(GalleryApplication::instance()->GetPicturesDir());
        album_collection_ = new AlbumCollection();
        event_collection_ = new EventCollection();
        preview_manager_ = new PreviewManager();

        collections_initialised = true;
    }
}

GalleryManager::~GalleryManager()
{
    if (resource_)
    {
        delete resource_;
        resource_ = NULL;
    }
}

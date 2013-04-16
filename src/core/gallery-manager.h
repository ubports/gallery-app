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
#ifndef GALLERYMANAGER_H
#define GALLERYMANAGER_H

#include <cstddef>
#include <QDir>

class QQuickView;

class Resource;
class GalleryStandardImageProvider;
class GalleryThumbnailImageProvider;
class Database;
class AlbumDefaultTemplate;
class MediaCollection;
class AlbumCollection;
class EventCollection;
class PreviewManager;
class GalleryManager;

/*!
 * Simple class which encapsulates instantiates objects which require only one instance.
 */
class GalleryManager
{
public:
    static GalleryManager* instance(const QDir& pictures_dir = QDir(),
                                    QQuickView *view = 0,
                                    const bool log_image_loading = false);

    /*!
     * Called after main loop is initialised. See GalleryApplication::exec() comments.
    */
    void post_init();

    Database* database() { return database_; }
    AlbumDefaultTemplate* album_default_template() { return default_template_; }
    MediaCollection* media_collection() { return media_collection_; }
    AlbumCollection* album_collection() { return album_collection_; }
    EventCollection* event_collection() { return event_collection_; }
    PreviewManager* preview_manager() { return preview_manager_; }
    Resource* resource() { return resource_; }
    GalleryStandardImageProvider* gallery_standard_image_provider() { return gallery_standard_image_provider_; }
    GalleryThumbnailImageProvider* gallery_thumbnail_image_provider() { return gallery_thumbnail_image_provider_; }

private:
    GalleryManager(const QDir& pictures_dir, QQuickView *view,
                   const bool log_image_loading);
    ~GalleryManager();

    GalleryManager(const GalleryManager&);
    void operator=(const GalleryManager&);

    static GalleryManager* gallery_mgr_;

    bool collections_initialised;

    Resource* resource_;
    GalleryStandardImageProvider* gallery_standard_image_provider_;
    GalleryThumbnailImageProvider* gallery_thumbnail_image_provider_;

    Database* database_;
    AlbumDefaultTemplate* default_template_;
    MediaCollection* media_collection_;
    AlbumCollection* album_collection_;
    EventCollection* event_collection_;
    PreviewManager* preview_manager_;

    const QDir pictures_dir_;
};

#endif // GALLERYMANAGER_H

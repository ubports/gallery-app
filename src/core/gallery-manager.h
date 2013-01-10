/*!
 * gallerymanager.h
 *
 * Simple class which encapsulates instantiates objects which require only one instance.
*/

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

#include <QDir>

//class PhotoMetadata;
class Resource;
class GalleryStandardImageProvider;
class GalleryThumbnailImageProvider;
class Database;
class AlbumDefaultTemplate;
class MediaCollection;
class AlbumCollection;
class EventCollection;
class PreviewManager;

class GalleryManager
{
public:
    static GalleryManager* GetInstance();

    /*!
     * Called after main loop is initialised. See GalleryApplication::exec() comments.
    */
    void post_init();

    //PhotoMetadata* GetPhotoMetaData() { return photo_meta_data_; }
    Database* database() { return database_; }
    AlbumDefaultTemplate* album_default_template() { return default_template_; }
    MediaCollection* media_collection() { return media_collection_; }
    AlbumCollection* album_collection() { return album_collection_; }
    EventCollection* event_collection() { return event_collection_; }
    PreviewManager* preview_manager() { return preview_manager_; }
    Resource* resource() { return resource_; }
    GalleryStandardImageProvider* gallery_standard_image_provider() { return gallery_standard_image_provider_; }
    GalleryThumbnailImageProvider* gallery_thumbnail_image_provider() { return gallery_thumbnail_image_provider_; }

    QDir& pictures_dir()  { return pictures_dir_; }
    bool is_portrait() { return is_portrait_; }
    bool is_fullscreen() { return is_fullscreen_; }
    bool startup_timer() { return startup_timer_; }

private:
    GalleryManager();
    ~GalleryManager();

    GalleryManager(GalleryManager const&);
    void operator=(GalleryManager const&);

    void process_args();
    void usage(bool error = false);
    void invalid_arg(QString arg);

    static GalleryManager* gallery_mgr_;

    bool collections_initialised;
    bool startup_timer_;
    bool is_fullscreen_;
    bool is_portrait_;
    QDir pictures_dir_;

    QHash<QString, QSize>& form_factors_;
    QString& form_factor_;
    bool log_image_loading_;

    Resource* resource_;
    GalleryStandardImageProvider* gallery_standard_image_provider_;
    GalleryThumbnailImageProvider* gallery_thumbnail_image_provider_;

    //PhotoMetadata* photo_meta_data_;
    Database* database_;
    AlbumDefaultTemplate* default_template_;
    MediaCollection* media_collection_;
    AlbumCollection* album_collection_;
    EventCollection* event_collection_;
    PreviewManager* preview_manager_;
};

#endif // GALLERYMANAGER_H

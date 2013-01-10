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
 * gallerymanager.h
 *
 * Simple class which encapsulates instantiates objects which require only one instance.
 *
 */
#ifndef GALLERYMANAGER_H
#define GALLERYMANAGER_H

class PhotoMetadata;
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
    void PostInit();

    //PhotoMetadata* GetPhotoMetaData() { return photo_meta_data_; }
    Database* GetDatabase() { return database_; }
    AlbumDefaultTemplate* GetAlbumDefaultTemplate() { return default_template_; }
    MediaCollection* GetMediaCollection() { return media_collection_; }
    AlbumCollection* GetAlbumCollection() { return album_collection_; }
    EventCollection* GetEventCollection() { return event_collection_; }
    PreviewManager* GetPreviewManager() { return preview_manager_; }
    Resource* GetResource() { return resource_; }
    GalleryStandardImageProvider* GetGalleryStandardImageProvider() { return gallery_standard_image_provider_; }
    GalleryThumbnailImageProvider* GetGalleryThumbnailImageProvider() { return gallery_thumbnail_image_provider; }

private:
    GalleryManager();
    ~GalleryManager();

    GalleryManager(GalleryManager const&);
    void operator=(GalleryManager const&);

    static GalleryManager* gallery_mgr_;

    bool collections_initialised;

    PhotoMetadata* photo_meta_data_;
    Database* database_;
    AlbumDefaultTemplate* default_template_;
    MediaCollection* media_collection_;
    AlbumCollection* album_collection_;
    EventCollection* event_collection_;
    PreviewManager* preview_manager_;
    Resource* resource_;
    GalleryStandardImageProvider* gallery_standard_image_provider_;
    GalleryThumbnailImageProvider* gallery_thumbnail_image_provider;
};

#endif // GALLERYMANAGER_H

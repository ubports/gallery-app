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

#include <cstddef>

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
    static GalleryManager* instance(const QDir& picturesDir = QDir(),
                                    QQuickView *view = 0,
                                    const bool logImageLoading = false);

    void postInit();

    Database* database() { return m_database; }
    AlbumDefaultTemplate* albumDefaultTemplate() { return m_defaultTemplate; }
    MediaCollection* mediaCollection() { return m_mediaCollection; }
    AlbumCollection* albumCollection() { return m_albumCollection; }
    EventCollection* eventCollection() { return m_eventCollection; }
    PreviewManager* previewManager() { return m_previewManager; }
    Resource* resource() { return m_resource; }
    GalleryStandardImageProvider* galleryStandardImageProvider() { return m_standardImageProvider; }
    GalleryThumbnailImageProvider* galleryThumbnailImageProvider() { return m_thumbnailImageProvider_; }

private:
    GalleryManager(const QDir& picturesDir, QQuickView *view, const bool logImageLoading);
    ~GalleryManager();

    GalleryManager(const GalleryManager&);
    void operator=(const GalleryManager&);

    void initPreviewManager();
    void fillMediaCollection();

    static GalleryManager* m_galleryManager;

    bool collectionsInitialised;

    Resource* m_resource;
    GalleryStandardImageProvider* m_standardImageProvider;
    GalleryThumbnailImageProvider* m_thumbnailImageProvider_;

    Database* m_database;
    AlbumDefaultTemplate* m_defaultTemplate;
    MediaCollection* m_mediaCollection;
    AlbumCollection* m_albumCollection;
    EventCollection* m_eventCollection;
    PreviewManager* m_previewManager;
};

#endif // GALLERYMANAGER_H

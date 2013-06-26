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
#include "media-table.h"

// event
#include "event-collection.h"

// media
#include "media-collection.h"

// qml
#include "gallery-standard-image-provider.h"
#include "gallery-thumbnail-image-provider.h"

GalleryManager* GalleryManager::m_galleryManager = NULL;

GalleryManager* GalleryManager::instance(const QString &picturesDir,
                                         QQuickView *view, const bool logImageLoading)
{
    if (!m_galleryManager)
        m_galleryManager = new GalleryManager(picturesDir, view, logImageLoading);

    return m_galleryManager;
}

GalleryManager::GalleryManager(const QString& picturesDir,
                               QQuickView *view, const bool logImageLoading)
    : collectionsInitialised(false),
      m_resource(0),
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
    Q_UNUSED(picturesDir);
    Q_UNUSED(view);
    Q_UNUSED(logImageLoading);
}

void GalleryManager::postInit()
{
    Q_ASSERT(m_resource);

    if (!collectionsInitialised)
    {
        m_defaultTemplate = new AlbumDefaultTemplate();
        m_mediaCollection = new MediaCollection();

        collectionsInitialised = true;
    }
}

AlbumCollection *GalleryManager::albumCollection()
{
    if (!m_albumCollection)
        m_albumCollection = new AlbumCollection();

    return m_albumCollection;
}

EventCollection *GalleryManager::eventCollection()
{
    if (!m_eventCollection)
        m_eventCollection = new EventCollection;

    return m_eventCollection;
}
GalleryManager::~GalleryManager()
{
    delete m_standardImageProvider;
    delete m_thumbnailImageProvider;
    delete m_defaultTemplate;
    delete m_mediaCollection;
    delete m_albumCollection;
    delete m_eventCollection;
}

void GalleryManager::onMediaItemAdded(QFileInfo file)
{
    Q_UNUSED(file);
}

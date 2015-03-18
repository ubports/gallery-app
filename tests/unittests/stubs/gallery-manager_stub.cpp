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

GalleryManager* GalleryManager::m_galleryManager = NULL;

GalleryManager::GalleryManager(bool desktopMode, const QString& picturesDir)
    : collectionsInitialised(false),
      m_resource(0),
      m_database(0),
      m_defaultTemplate(0),
      m_mediaCollection(0),
      m_albumCollection(0),
      m_eventCollection(0),
      m_monitor(0),
      m_mediaLibrary(0)
{
    Q_UNUSED(picturesDir);
}

GalleryManager* GalleryManager::instance()
{
    return m_galleryManager;
}

void GalleryManager::postInit()
{
    Q_ASSERT(m_resource);

    if (!collectionsInitialised)
    {
        m_defaultTemplate = new AlbumDefaultTemplate();
        m_mediaCollection = new MediaCollection(0);

        collectionsInitialised = true;
    }
}

AlbumCollection *GalleryManager::albumCollection()
{
    return m_albumCollection;
}

EventCollection *GalleryManager::eventCollection()
{
    return m_eventCollection;
}

QmlMediaCollectionModel *GalleryManager::mediaLibrary() const
{
    return m_mediaLibrary;
}

GalleryManager::~GalleryManager()
{
    delete m_defaultTemplate;
    delete m_mediaCollection;
    delete m_albumCollection;
    delete m_eventCollection;
}

void GalleryManager::onMediaItemAdded(QString file, int priority)
{
    Q_UNUSED(file);
}

void GalleryManager::onMediaItemRemoved(qint64 mediaId)
{
    Q_UNUSED(mediaId);
}

void GalleryManager::onMediaObjectCreated(MediaSource *mediaObject)
{
    Q_UNUSED(mediaObject);
}

void GalleryManager::onMediaFromDBLoaded(QSet<DataObject *> mediaFromDB)
{
    Q_UNUSED(mediaFromDB);
}

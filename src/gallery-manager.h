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

// media
#include "media-source.h"

#include <QFileInfo>
#include <QObject>

#include <cstddef>

class QQuickView;

class AlbumCollection;
class AlbumDefaultTemplate;
class Database;
class EventCollection;
class GalleryManager;
class MediaCollection;
class MediaMonitor;
class MediaObjectFactory;
class QmlMediaCollectionModel;
class Resource;

/*!
 * Simple class which encapsulates instantiates objects which require only one instance.
 */
class GalleryManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QmlMediaCollectionModel* mediaLibrary READ mediaLibrary NOTIFY mediaLibraryChanged)

public:
    GalleryManager(bool desktopMode, const QString &picturesDir);
    ~GalleryManager();

    static GalleryManager* instance();

    void enableContentLoadFilter(MediaSource::MediaType filterType);
    void postInit();

    Database *database() { return m_database; }
    AlbumDefaultTemplate *albumDefaultTemplate() { return m_defaultTemplate; }
    MediaCollection *mediaCollection() { return m_mediaCollection; }
    AlbumCollection *albumCollection();
    EventCollection *eventCollection();
    Resource *resource() { return m_resource; }

    void logImageLoading(bool log);

    QmlMediaCollectionModel *mediaLibrary() const;

signals:
    void mediaLibraryChanged();
    void consistencyCheckFinished();
    void collectionChanged();

private slots:
    void onMediaItemAdded(QString file, int priority);
    void onMediaItemRemoved(qint64 mediaId);
    void onMediaObjectCreated(MediaSource *mediaObject);
    void onMediaFromDBLoaded(QSet<DataObject *> mediaFromDB);

private:
    GalleryManager(const GalleryManager&);
    void operator=(const GalleryManager&);

    void fillMediaCollection();
    void startFileMonitoring();

    static GalleryManager* m_galleryManager;

    bool collectionsInitialised;

    Resource* m_resource;
    Database* m_database;
    AlbumDefaultTemplate* m_defaultTemplate;
    MediaCollection* m_mediaCollection;
    AlbumCollection* m_albumCollection;
    EventCollection* m_eventCollection;
    MediaObjectFactory *m_mediaFactory;
    MediaMonitor *m_monitor;
    bool m_desktopMode;

    mutable QmlMediaCollectionModel *m_mediaLibrary;
};

#endif // GALLERYMANAGER_H

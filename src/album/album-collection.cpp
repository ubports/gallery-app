/*
 * Copyright (C) 2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 */

#include "album-collection.h"
#include "album-template.h"

// database
#include "album-table.h"

// media
#include "media-collection.h"

/*!
 * \brief AlbumCollection::AlbumCollection
 */
AlbumCollection::AlbumCollection(MediaCollection *mediaCollection,
                                 AlbumTable *albumTable, AlbumTemplate *albumTemplate)
    : ContainerSourceCollection("AlbumCollection", creationDateTimeDescendingComparator),
      m_mediaCollection(mediaCollection),
      m_albumTable(albumTable)
{
    // Load existing albums from database.
    QList<Album*> album_list;
    m_albumTable->getAlbums(&album_list);
    foreach (Album* a, album_list) {
        a->setAlbumTemplate(albumTemplate);
        add(a);
        int saved_current_page = a->currentPage();

        // Link each album up with its photos.
        QSet<DataObject*> photos;
        QList<qint64> photo_list;
        MediaSource *media;
        m_albumTable->mediaForAlbum(a->id(), &photo_list);
        foreach (qint64 mediaId, photo_list) {
            media = m_mediaCollection->mediaForId(mediaId);
            if (media)
                photos.insert(media);
        }

        a->attachMany(photos);

        // After photos are attached, restore the current page.
        a->setCurrentPage(saved_current_page);

        // If there are no photos in the album, mark it as closed.
        // This is needed for the case where the user exits the application while
        // viewing an empty album.
        if (a->containedCount() == 0)
            a->setClosed(true);
    }

    // We need to monitor the media collection so that when photos get removed
    // from the system, they also get removed from all albums.
    QObject::connect(
                m_mediaCollection,
                SIGNAL(contentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)),
                this,
                SLOT(onMediaAddedRemoved(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)));
}

/*!
 * \brief AlbumCollection::creationDateTimeAscendingComparator
 * \param a
 * \param b
 * \return
 */
bool AlbumCollection::creationDateTimeAscendingComparator(DataObject* a, DataObject* b)
{
    return qobject_cast<Album*>(a)->creationDateTime() < qobject_cast<Album*>(b)->creationDateTime();
}

/*!
 * \brief AlbumCollection::creationDateTimeDescendingComparator
 * \param a
 * \param b
 * \return
 */
bool AlbumCollection::creationDateTimeDescendingComparator(DataObject* a, DataObject* b)
{
    return creationDateTimeAscendingComparator(b, a);
}

/*!
 * \brief AlbumCollection::notifyAlbumCurrentPageContentsChanged
 * \param album
 */
void AlbumCollection::notifyAlbumCurrentPageContentsChanged(Album* album)
{
    emit albumCurrentPageContentsChanged(album);
}

/*!
 * \brief AlbumCollection::onMediaAddedRemoved
 * \param added
 * \param removed
 */
void AlbumCollection::onMediaAddedRemoved(const QSet<DataObject *> *added,
                                             const QSet<DataObject *> *removed,
                                             bool notify)
{
    if (removed != NULL) {
        // TODO: this could maybe be optimized.  Many albums might not care about
        // the particular photo being added.
        DataObject* album_object;
        foreach (album_object, getAsSet()) {
            Album* album = qobject_cast<Album*>(album_object);
            Q_ASSERT(album != NULL);

            DataObject* object;
            foreach (object, *removed) {
                MediaSource* media = qobject_cast<MediaSource*>(object);
                Q_ASSERT(media != NULL);

                album->detach(media, true);
            }
        }
    }
}

/*!
 * \brief AlbumCollection::notifyContentsChanged
 * \param added
 * \param removed
 */
void AlbumCollection::notifyContentsChanged(const QSet<DataObject*>* added,
                                              const QSet<DataObject*>* removed,
                                              const bool notify)
{
    ContainerSourceCollection::notifyContentsChanged(added, removed, notify);

    if (added != NULL) {
        foreach (DataObject* object, *added) {
            Album* album = qobject_cast<Album*>(object);
            Q_ASSERT(album != NULL);

            // Add the album.
            m_albumTable->addAlbum(album);

            // Add initial photos.
            foreach(DataObject* o, album->contained()->getAll()) {
                MediaSource* media = qobject_cast<MediaSource*>(o);
                Q_ASSERT(media != NULL);
                m_albumTable->attachToAlbum(album->id(), media->id());
            }
        }
    }

    if (removed != NULL) {
        foreach (DataObject* object, *removed) {
            Album* album = qobject_cast<Album*>(object);
            Q_ASSERT(album != NULL);

            m_albumTable->removeAlbum(album);
        }
    }
}

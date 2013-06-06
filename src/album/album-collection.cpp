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
#include "media-collection.h"
#include "database.h"
#include "album-table.h"
#include "gallery-manager.h"

/*!
 * \brief AlbumCollection::AlbumCollection
 */
AlbumCollection::AlbumCollection()
    : ContainerSourceCollection("AlbumCollection", CreationDateTimeDescendingComparator)
{
    // Load existing albums from database.
    QList<Album*> album_list;
    GalleryManager::instance()->database()->get_album_table()->get_albums(&album_list);
    foreach (Album* a, album_list) {
        Add(a);
        int saved_current_page = a->current_page();

        // Link each album up with its photos.
        QSet<DataObject*> photos;
        QList<qint64> photo_list;
        GalleryManager::instance()->database()->get_album_table()->media_for_album(a->get_id(), &photo_list);
        foreach (qint64 mediaId, photo_list)
            photos.insert(GalleryManager::instance()->media_collection()->mediaForId(mediaId));

        a->AttachMany(photos);

        // After photos are attached, restore the current page.
        a->set_current_page(saved_current_page);

        // If there are no photos in the album, mark it as closed.
        // This is needed for the case where the user exits the application while
        // viewing an empty album.
        if (a->ContainedCount() == 0)
            a->set_closed(true);
    }

    // We need to monitor the media collection so that when photos get removed
    // from the system, they also get removed from all albums.
    QObject::connect(
                GalleryManager::instance()->media_collection(),
                SIGNAL(contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
                this,
                SLOT(on_media_added_removed(const QSet<DataObject*>*,const QSet<DataObject*>*)));
}

/*!
 * \brief AlbumCollection::CreationDateTimeAscendingComparator
 * \param a
 * \param b
 * \return
 */
bool AlbumCollection::CreationDateTimeAscendingComparator(DataObject* a, DataObject* b)
{
    return qobject_cast<Album*>(a)->creation_date_time() < qobject_cast<Album*>(b)->creation_date_time();
}

/*!
 * \brief AlbumCollection::CreationDateTimeDescendingComparator
 * \param a
 * \param b
 * \return
 */
bool AlbumCollection::CreationDateTimeDescendingComparator(DataObject* a, DataObject* b)
{
    return CreationDateTimeAscendingComparator(b, a);
}

/*!
 * \brief AlbumCollection::notify_album_current_page_contents_altered
 * \param album
 */
void AlbumCollection::notify_album_current_page_contents_altered(Album* album)
{
    emit album_current_page_contents_altered(album);
}

/*!
 * \brief AlbumCollection::on_media_added_removed
 * \param added
 * \param removed
 */
void AlbumCollection::on_media_added_removed(const QSet<DataObject *> *added,
                                             const QSet<DataObject *> *removed)
{
    if (removed != NULL) {
        // TODO: this could maybe be optimized.  Many albums might not care about
        // the particular photo being added.
        DataObject* album_object;
        foreach (album_object, GetAsSet()) {
            Album* album = qobject_cast<Album*>(album_object);
            Q_ASSERT(album != NULL);

            DataObject* object;
            foreach (object, *removed) {
                MediaSource* media = qobject_cast<MediaSource*>(object);
                Q_ASSERT(media != NULL);

                album->Detach(media);
            }
        }
    }
}

/*!
 * \brief AlbumCollection::notify_contents_altered
 * \param added
 * \param removed
 */
void AlbumCollection::notify_contents_altered(const QSet<DataObject*>* added,
                                              const QSet<DataObject*>* removed)
{
    ContainerSourceCollection::notify_contents_altered(added, removed);

    if (added != NULL) {
        foreach (DataObject* object, *added) {
            Album* album = qobject_cast<Album*>(object);
            Q_ASSERT(album != NULL);

            // Add the album.
            GalleryManager::instance()->database()->get_album_table()->add_album(album);

            // Add initial photos.
            foreach(DataObject* o, album->contained()->GetAll()) {
                MediaSource* media = qobject_cast<MediaSource*>(o);
                Q_ASSERT(media != NULL);
                GalleryManager::instance()->database()->get_album_table()->attach_to_album(album->get_id(), media->get_id());
            }
        }
    }

    if (removed != NULL) {
        foreach (DataObject* object, *removed) {
            Album* album = qobject_cast<Album*>(object);
            Q_ASSERT(album != NULL);

            GalleryManager::instance()->database()->get_album_table()->remove_album(album);
        }
    }
}

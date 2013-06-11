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

#include "media-collection.h"
#include "media-source.h"

// database
#include "database.h"
#include "media-table.h"

// src
#include "gallery-manager.h"

#include <QFileInfo>
#include <QString>
#include <QStringList>

/*!
 * \brief MediaCollection::MediaCollection
 * \param directory
 */
MediaCollection::MediaCollection()
    : SourceCollection("MediaCollection")
{
    // By default, sort all media by its exposure date time, descending
    setComparator(ExposureDateTimeDescendingComparator);
}

/*!
 * \brief MediaCollection::ExposureDateTimeAscendingComparator
 * NOTE: this comparator function expects the API contract of
 *       DataObject::number() to return the same value for the same logical
 *       data object across invocations of Gallery. Right now, this contract
 *       is tenuously maintained. See the TODO item in DataObject.h.
 * \param a
 * \param b
 * \return
 */
bool MediaCollection::ExposureDateTimeAscendingComparator(DataObject* a,
                                                          DataObject* b)
{
    QDateTime exptime_a = qobject_cast<MediaSource*>(a)->exposureDateTime();
    QDateTime exptime_b = qobject_cast<MediaSource*>(b)->exposureDateTime();

    return (exptime_a == exptime_b) ?
                (DataCollection::defaultDataObjectComparator(a, b)) :
                (exptime_a < exptime_b);
}

/*!
 * \brief MediaCollection::ExposureDateTimeDescendingComparator
 * \param a
 * \param b
 * \return
 */
bool MediaCollection::ExposureDateTimeDescendingComparator(DataObject* a,
                                                           DataObject* b)
{
    return !ExposureDateTimeAscendingComparator(a, b);
}

/*!
 * \brief MediaCollection::mediaForId Returns a media object for a row id.
 * \param id
 * \return Returns a media object for a row id.
 */
MediaSource* MediaCollection::mediaForId(qint64 id)
{
    return id_map_.contains(id) ? qobject_cast<MediaSource*>(id_map_[id]) : NULL;
}

/*!
 * \brief MediaCollection::notify_contents_altered
 * \param added
 * \param removed
 */
void MediaCollection::notifyContentsChanged(const QSet<DataObject*>* added,
                                              const QSet<DataObject*>* removed)
{
    SourceCollection::notifyContentsChanged(added, removed);

    // Track IDs of objects as they're added and removed.
    if (added != NULL) {
        QSetIterator<DataObject*> i(*added);
        while (i.hasNext()) {
            DataObject* o = i.next();
            id_map_.insert(qobject_cast<MediaSource*>(o)->id(), o);

            MediaSource* media = qobject_cast<MediaSource*>(o);
            if (media != 0) {
                m_fileMediaMap.insert(media->file().absoluteFilePath(), media);
            }
        }
    }

    if (removed != NULL) {
        QSetIterator<DataObject*> i(*removed);
        while (i.hasNext()) {
            DataObject* o = i.next();
            MediaSource* media = qobject_cast<MediaSource*>(o);

            if (media != 0) {
                m_fileMediaMap.remove(media->file().absoluteFilePath());
            }

            id_map_.remove(media->id());

            // TODO: In the future we may want to do this in the Destroy method
            // (as defined in DataSource) if we want to differentiate between
            // removing the photo and "deleting the backing file."
            GalleryManager::instance()->database()->get_media_table()->remove(media->id());
        }
    }
}

/*!
 * \brief MediaCollection::photoFromFileinfo
 * Returns an existing photo object if we've already loaded one
 * for this file, or NULL otherwise. Used for preventing duplicates
 * from appearing after an edit.
 * \param file_to_load
 * \return
 */
MediaSource *MediaCollection::mediaFromFileinfo(const QFileInfo& file)
{
    return m_fileMediaMap.value(file.absoluteFilePath(), 0);
}

/*!
 * \reimp
 */
void MediaCollection::addMany(const QSet<DataObject *> &objects)
{
    foreach (DataObject* data, objects) {
        MediaSource* media = qobject_cast<MediaSource*>(data);
        id_map_.insert(media->id(), media);
    }

    DataCollection::addMany(objects);
}

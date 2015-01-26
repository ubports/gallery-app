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

#include <QFileInfo>
#include <QString>
#include <QStringList>

/*!
 * \brief MediaCollection::MediaCollection
 * \param directory
 */
MediaCollection::MediaCollection(MediaTable *mediaTable)
    : SourceCollection("MediaCollection"),
      m_mediaTable(mediaTable)
{
    // By default, sort all media by its exposure date time, descending
    setComparator(exposureDateTimeDescendingComparator);
}

/*!
 * \brief MediaCollection::exposureDateTimeAscendingComparator
 * NOTE: this comparator function expects the API contract of
 *       DataObject::number() to return the same value for the same logical
 *       data object across invocations of Gallery. Right now, this contract
 *       is tenuously maintained. See the TODO item in DataObject.h.
 * \param a
 * \param b
 * \return
 */
bool MediaCollection::exposureDateTimeAscendingComparator(DataObject* a,
                                                          DataObject* b)
{
    const QDateTime &exptime_a = qobject_cast<MediaSource*>(a)->exposureDateTime();
    const QDateTime &exptime_b = qobject_cast<MediaSource*>(b)->exposureDateTime();

    return (exptime_a == exptime_b) ?
                (DataCollection::defaultDataObjectComparator(a, b)) :
                (exptime_a < exptime_b);
}

/*!
 * \brief MediaCollection::exposureDateTimeDescendingComparator
 * \param a
 * \param b
 * \return
 */
bool MediaCollection::exposureDateTimeDescendingComparator(DataObject* a,
                                                           DataObject* b)
{
    return !exposureDateTimeAscendingComparator(a, b);
}

/*!
 * \brief MediaCollection::mediaForId Returns a media object for a row id.
 * \param id
 * \return Returns a media object for a row id.
 */
MediaSource* MediaCollection::mediaForId(qint64 id)
{
    return m_idMap.contains(id) ? qobject_cast<MediaSource*>(m_idMap[id]) : NULL;
}

/*!
 * \brief MediaCollection::notifyContentsChanged
 * \param added
 * \param removed
 */
void MediaCollection::notifyContentsChanged(const QSet<DataObject*>* added,
                                            const QSet<DataObject*>* removed,
                                            bool notify)
{
    SourceCollection::notifyContentsChanged(added, removed, notify);

    // Track IDs of objects as they're added and removed.
    if (added != NULL) {
        QSetIterator<DataObject*> i(*added);
        while (i.hasNext()) {
            DataObject* o = i.next();
            m_idMap.insert(qobject_cast<MediaSource*>(o)->id(), o);

            MediaSource* media = qobject_cast<MediaSource*>(o);
            if (media != 0) {
                m_fileMediaMap.insert(media->file().absoluteFilePath(), media);
                QObject::connect(media, SIGNAL(busyChanged(bool)),
                                 this, SIGNAL(mediaIsBusy(bool)));
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
                QObject::disconnect(media, SIGNAL(busyChanged(bool)),
                                    this, SIGNAL(mediaIsBusy(bool)));
            }

            m_idMap.remove(media->id());

            // TODO: In the future we may want to do this in the Destroy method
            // (as defined in DataSource) if we want to differentiate between
            // removing the photo and "deleting the backing file."
            m_mediaTable->remove(media->id());
        }
    }

    if (added || removed)
        emit collectionChanged();
}

/*!
 * \brief MediaCollection::photoFromFileinfo
 * Returns an existing photo object if we've already loaded one
 * for this file, or NULL otherwise. Used for preventing duplicates
 * from appearing after an edit.
 * \param file_to_load
 * \return
 */
const MediaSource *MediaCollection::mediaFromFileinfo(const QFileInfo& file) const
{
    return m_fileMediaMap.value(file.absoluteFilePath(), 0);
}

/*!
 * \brief MediaCollection::containsFile
 * \param filename
 * \return
 */
bool MediaCollection::containsFile(const QString &filename) const
{
    return m_fileMediaMap.contains(filename);
}

/*!
 * \reimp
 */
void MediaCollection::add(DataObject *object)
{
    MediaSource* media = qobject_cast<MediaSource*>(object);
    if (media->file().exists()) {
        m_idMap.insert(media->id(), media);
        DataCollection::add(object);
    } else {
        m_mediaTable->remove(media->id());
        media->deleteLater();
    }
}

/*!
+ * \reimp
+ */
void MediaCollection::addMany(const QSet<DataObject *> &objects)
{
    foreach (DataObject* data, objects) {
        MediaSource* media = qobject_cast<MediaSource*>(data);
        m_idMap.insert(media->id(), media);
    }

    DataCollection::addMany(objects);
}

/*!
 * \brief MediaCollection::destroy
 * \param media
 */
void MediaCollection::destroy(MediaSource *media, bool destroy_backing)
{
    SourceCollection::destroy(media, destroy_backing, true);
}

/*!
 * \brief MediaCollection::remove
 * \param id
 */
void MediaCollection::destroy(qint64 id, bool destroy_backing)
{
    if (m_idMap.contains(id)) {
        MediaSource *media = qobject_cast<MediaSource*>(m_idMap[id]);
        SourceCollection::destroy(media, destroy_backing, true);
    }
}

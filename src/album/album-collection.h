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

#ifndef GALLERY_ALBUM_COLLECTION_H_
#define GALLERY_ALBUM_COLLECTION_H_

#include "album.h"

// core
#include "container-source-collection.h"
#include "data-object.h"

#include <QObject>

class AlbumTable;
class AlbumTemplate;
class MediaCollection;

/*!
 * \brief The AlbumCollection class
 */
class AlbumCollection : public ContainerSourceCollection
{
    Q_OBJECT

signals:
    void albumCurrentPageContentsChanged(Album* album);

public:
    friend class Album;

    AlbumCollection(MediaCollection *mediaCollection, AlbumTable *albumTable,
                    AlbumTemplate *albumTemplate);

    static bool creationDateTimeAscendingComparator(DataObject* a, DataObject* b);
    static bool creationDateTimeDescendingComparator(DataObject* a, DataObject* b);

protected:
    virtual void notifyAlbumCurrentPageContentsChanged(Album* album);

    virtual void notifyContentsChanged(const QSet<DataObject*>* added,
                                       const QSet<DataObject*>* removed,
                                       bool notify);

private slots:
    void onMediaAddedRemoved(const QSet<DataObject*>* added,
                             const QSet<DataObject*>* removed,
                             bool notify);

private:
    MediaCollection *m_mediaCollection;
    AlbumTable *m_albumTable;
};

#endif  // GALLERY_ALBUM_COLLECTION_H_

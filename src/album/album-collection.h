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

/*!
 * \brief The AlbumCollection class
 */
class AlbumCollection : public ContainerSourceCollection
{
    Q_OBJECT

signals:
    void album_current_page_contents_altered(Album* album);

public:
    friend class Album;

    AlbumCollection();

    static bool CreationDateTimeAscendingComparator(DataObject* a, DataObject* b);
    static bool CreationDateTimeDescendingComparator(DataObject* a, DataObject* b);

protected:
    virtual void notify_album_current_page_contents_altered(Album* album);

    virtual void notify_contents_altered(const QSet<DataObject*>* added,
                                         const QSet<DataObject*>* removed);

private slots:
    void on_media_added_removed(const QSet<DataObject*>* added,
                                const QSet<DataObject*>* removed);
};

#endif  // GALLERY_ALBUM_COLLECTION_H_

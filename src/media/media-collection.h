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

#ifndef GALLERY_MEDIA_COLLECTION_H_
#define GALLERY_MEDIA_COLLECTION_H_

#include <QFileInfo>
#include <QHash>
#include <QSet>

// core
#include "source-collection.h"

class DataObject;
class MediaSource;

/*!
 * \brief The MediaCollection class
 */
class MediaCollection : public SourceCollection
{
    Q_OBJECT

public:
    MediaCollection();

    static bool ExposureDateTimeAscendingComparator(DataObject* a, DataObject* b);
    static bool ExposureDateTimeDescendingComparator(DataObject* a, DataObject* b);

    MediaSource* mediaForId(qint64 id);
    MediaSource* photoFromFileinfo(const QFileInfo &file_to_load);

    virtual void AddMany(const QSet<DataObject*>& objects);

protected slots:
    virtual void notify_contents_altered(const QSet<DataObject*>* added,
                                         const QSet<DataObject*>* removed);

private:
    // Used by photoFromFileinfo() to prevent ourselves from accidentally
    // seeing a duplicate photo after an edit.
    QHash<QString, MediaSource*> m_filePhotoMap;

    QHash<qint64, DataObject*> id_map_;
};

#endif  // GALLERY_MEDIA_COLLECTION_H_

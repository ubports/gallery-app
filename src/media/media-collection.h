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
class MediaTable;

/*!
 * \brief The MediaCollection class
 */
class MediaCollection : public SourceCollection
{
    Q_OBJECT

public:
    MediaCollection(MediaTable *mediaTable);

    static bool exposureDateTimeAscendingComparator(DataObject* a, DataObject* b);
    static bool exposureDateTimeDescendingComparator(DataObject* a, DataObject* b);

    MediaSource* mediaForId(qint64 id);
    const MediaSource* mediaFromFileinfo(const QFileInfo &file) const;
    bool containsFile(const QString& filename) const;

    virtual void add(DataObject* object);
    virtual void addMany(const QSet<DataObject*>& objects);

    void destroy(MediaSource *media, bool destroy_backing);
    void destroy(qint64 id, bool destroy_backing);

signals:
    void mediaIsBusy(bool busy);
    void collectionChanged();

protected slots:
    virtual void notifyContentsChanged(const QSet<DataObject*>* added,
                                       const QSet<DataObject*>* removed,
                                       bool notify);

private:
    // Used by photoFromFileinfo() to prevent ourselves from accidentally
    // seeing a duplicate photo after an edit.
    QHash<QString, MediaSource*> m_fileMediaMap;
    QHash<qint64, DataObject*> m_idMap;
    MediaTable *m_mediaTable;
};

#endif  // GALLERY_MEDIA_COLLECTION_H_

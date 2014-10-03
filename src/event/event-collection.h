/*
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef GALLERY_EVENT_COLLECTION_H_
#define GALLERY_EVENT_COLLECTION_H_

#include "source-collection.h"

#include <QDate>
#include <QHash>
#include <QSet>

class DataObject;
class Event;
class MediaSource;

/*!
 * \brief The EventCollection class
 */
class EventCollection : public SourceCollection
{
    Q_OBJECT

public:
    EventCollection();

    Event* eventForDate(const QDate& date) const;
    Event* eventForMediaSource(MediaSource* media) const;

public slots:
    void onMediaAddedRemoved(const QSet<DataObject*>* added,
                             const QSet<DataObject*>* removed,
                             bool notify);

protected:
    virtual void notifyContentsChanged(const QSet<DataObject *> *added,
                                       const QSet<DataObject *> *removed,
                                       bool notify);

private:
    static bool comparator(DataObject* a, DataObject* b);

    QHash<QDate, Event*> m_dateMap;
};

#endif  // GALLERY_EVENT_COLLECTION_H_

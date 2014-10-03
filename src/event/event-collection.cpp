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

#include "event-collection.h"
#include "event.h"

// core
#include "data-object.h"

// media
#include "media-source.h"

/*!
 * \brief EventCollection::EventCollection
 */
EventCollection::EventCollection()
    : SourceCollection("EventCollection")
{
    setComparator(comparator);
}

/*!
 * \brief EventCollection::eventForDate
 * \param date
 * \return
 */
Event* EventCollection::eventForDate(const QDate& date) const
{
    return m_dateMap.value(date);
}

/*!
 * \brief EventCollection::eventForMediaSource
 * \param media
 * \return
 */
Event* EventCollection::eventForMediaSource(MediaSource* media) const
{
    // TODO: Could use lookup table here, but this is fine for now
    Event* event;
    foreach (event, getAllAsType<Event*>()) {
        if (event->contains(media))
            return event;
    }

    return NULL;
}

/*!
 * \brief EventCollection::comparator sorts Events in reverse chronological order
 * \param a
 * \param b
 * \return
 */
bool EventCollection::comparator(DataObject* a, DataObject* b)
{
    Event* eventa = qobject_cast<Event*>(a);
    Q_ASSERT(eventa != NULL);

    Event* eventb = qobject_cast<Event*>(b);
    Q_ASSERT(eventb != NULL);

    return eventa->date() > eventb->date();
}

/*!
 * \brief EventCollection::onMediaAddedRemoved
 * \param added
 * \param removed
 */
void EventCollection::onMediaAddedRemoved(const QSet<DataObject *> *added,
                                           const QSet<DataObject *> *removed,
                                           bool notify)
{
    if (added != NULL) {
        DataObject* object;
        foreach (object, *added) {
            MediaSource* media = qobject_cast<MediaSource*>(object);
            Q_ASSERT(media != NULL);

            Event* existing = m_dateMap.value(media->exposureDate());
            if (existing == NULL) {
                existing = new Event(this, media->exposureDate());

                add(existing);
            }

            existing->attach(media);
        }
    }

    if (removed != NULL) {
        DataObject* object;
        foreach (object, *removed) {
            MediaSource* media = qobject_cast<MediaSource*>(object);
            Q_ASSERT(media != NULL);

            Event* event = m_dateMap.value(media->exposureDate());
            Q_ASSERT(event != NULL);

            event->detach(media, false);

            if (event->containedCount() == 0)
                destroy(event, true, true);
        }
    }
}

/*!
 * \brief EventCollection::notifyContentsChanged
 * \param added
 * \param removed
 */
void EventCollection::notifyContentsChanged(const QSet<DataObject *> *added,
                                            const QSet<DataObject *> *removed,
                                            bool notify)
{
    if (added != NULL) {
        DataObject* object;
        foreach (object, *added) {
            Event* event = qobject_cast<Event*>(object);
            Q_ASSERT(event != NULL);

            // One Event per day
            Q_ASSERT(!m_dateMap.contains(event->date()));
            m_dateMap.insert(event->date(), event);
        }
    }

    if (removed != NULL) {
        DataObject* object;
        foreach (object, *removed) {
            Event* event = qobject_cast<Event*>(object);
            Q_ASSERT(event != NULL);

            Q_ASSERT(m_dateMap.contains(event->date()));
            m_dateMap.remove(event->date());
        }
    }

    SourceCollection::notifyContentsChanged(added, removed, notify);
}

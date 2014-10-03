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

#include "qml-event-overview-model.h"
#include "data-object.h"
#include "selectable-view-collection.h"
#include "event.h"
#include "event-collection.h"
#include "variants.h"
#include "gallery-manager.h"

/*!
 * \brief QmlEventOverviewModel::QmlEventOverviewModel
 * \param parent
 */
QmlEventOverviewModel::QmlEventOverviewModel(QObject* parent)
    : QmlMediaCollectionModel(parent, descendingComparator),
      m_ascendingOrder(false), m_syncingMedia(false)
{
    // initialize ViewCollection as it stands now with Events
    monitorNewViewCollection();

    // We need to know when events get removed from the system so we can remove
    // them too.
    QObject::connect(
                GalleryManager::instance()->eventCollection(),
                SIGNAL(contentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)),
                this,
                SLOT(onEventsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)));
}

/*!
 * \brief QmlEventOverviewModel::ascendingOrder
 * \return
 */
bool QmlEventOverviewModel::ascendingOrder() const
{
    return m_ascendingOrder;
}

/*!
 * \brief QmlEventOverviewModel::setAscendingOrder
 * \param ascending
 */
void QmlEventOverviewModel::setAscendingOrder(bool ascending)
{
    if (ascending == m_ascendingOrder)
        return;

    m_ascendingOrder = ascending;
    DataObjectComparator comparator =
            m_ascendingOrder ? ascendingComparator : descendingComparator;

    setDefaultComparator(comparator);
    backingViewCollection()->setComparator(comparator);

    emit ascendingChanged();
}

/*!
 * \brief QmlEventOverviewModel::toVariant
 * \param object
 * \return
 */
QVariant QmlEventOverviewModel::toVariant(DataObject* object) const
{
    Event* event = qobject_cast<Event*>(object);

    return (event != NULL) ? QVariant::fromValue(event) : QmlMediaCollectionModel::toVariant(object);
}

/*!
 * \brief QmlEventOverviewModel::fromVariant
 * \param var
 * \return
 */
DataObject* QmlEventOverviewModel::fromVariant(QVariant var) const
{
    DataObject* object = UncheckedVariantToObject<Event*>(var);

    return (object != NULL) ? object : QmlMediaCollectionModel::fromVariant(var);
}

/*!
 * \brief QmlEventOverviewModel::notifyBackingCollectionChanged
 */
void QmlEventOverviewModel::notifyBackingCollectionChanged()
{
    monitorNewViewCollection();

    QmlViewCollectionModel::notifyBackingCollectionChanged();
}

/*!
 * \brief QmlEventOverviewModel::monitorNewViewCollection
 */
void QmlEventOverviewModel::monitorNewViewCollection()
{
    if (backingViewCollection() == NULL)
        return;

    QObject::connect(
                backingViewCollection(),
                SIGNAL(contentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)),
                this,
                SLOT(onEventOverviewContentsChanged(const QSet<DataObject*>*,const QSet<DataObject*>*, bool)));

    QObject::connect(
                backingViewCollection(),
                SIGNAL(selectionChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)),
                this,
                SLOT(onEventOverviewSelectionChanged(const QSet<DataObject*>*,const QSet<DataObject*>*)));

    // seed existing contents with Events
    onEventOverviewContentsChanged(&backingViewCollection()->getAsSet(), NULL, true);
}

/*!
 * \brief QmlEventOverviewModel::onEventsChanged
 * \param added
 * \param removed
 */
void QmlEventOverviewModel::onEventsChanged(const QSet<DataObject*>* added,
                                            const QSet<DataObject*>* removed,
                                            bool notify)
{
    SelectableViewCollection* view = backingViewCollection();
    if (view == NULL)
        return;

    if (removed != NULL) {
        DataObject* object;
        foreach (object, *removed) {
            Event* event = qobject_cast<Event*>(object);
            Q_ASSERT(event != NULL);

            if (view->contains(event))
                view->remove(event, true);
        }
    }
}

bool QmlEventOverviewModel::isAccepted(DataObject* item)
{
    Q_UNUSED(item);
    return true;
}

/*!
 * \brief QmlEventOverviewModel::onEventOverviewContentsChanged
 * \param added
 * \param removed
 */
void QmlEventOverviewModel::onEventOverviewContentsChanged(
        const QSet<DataObject*>* added, const QSet<DataObject*>* removed, bool notify)
{
    SelectableViewCollection* view = backingViewCollection();

    if (added != NULL) {
        DataObject* object;
        foreach (object, *added) {
            MediaSource* source = qobject_cast<MediaSource*>(object);
            if (source == NULL)
                continue;

            QDate source_date = source->exposureDateTime().date();
            Event* event = GalleryManager::instance()->eventCollection()->eventForDate(source_date);
            Q_ASSERT(event != NULL);

            if (!view->contains(event))
                view->add(event);
        }
    }
}

/*!
 * \brief QmlEventOverviewModel::onEventOverviewSelectionChanged
 * \param selected
 * \param unselected
 */
void QmlEventOverviewModel::onEventOverviewSelectionChanged(
        const QSet<DataObject*>* selected, const QSet<DataObject*>* unselected)
{
    // if an Event is selected, select all photos in that date
    syncSelectedMedia(selected, true);

    // if an Event is unselected, unselect all photos in that date
    syncSelectedMedia(unselected, false);
}

/*!
 * \brief QmlEventOverviewModel::syncSelectedMedia
 * \param toggled
 * \param selected
 */
void QmlEventOverviewModel::syncSelectedMedia(const QSet<DataObject*>* toggled,
                                              bool selected)
{
    if (toggled == NULL)
        return;

    // Don't recurse -- only take action from the selection made by the user, not
    // any other selections we've done internally.
    if (m_syncingMedia)
        return;

    m_syncingMedia = true;

    SelectableViewCollection* view = backingViewCollection();
    int count = view->count();

    // Walk the toggle group looking for Event's; when found, walk all the
    // MediaSources that follow and select or unselect them; when another
    // Event is found (or end of list), exit
    //
    // TODO: Select/Unselect in bulk operations for efficiency
    DataObject* object;
    foreach (object, *toggled) {
        Event* event = qobject_cast<Event*>(object);
        if (event == NULL)
            continue;

        int index = view->indexOf(event);
        for (int ctr = index + 1; ctr < count; ctr++) {
            MediaSource* media = qobject_cast<MediaSource*>(view->getAt(ctr));
            if (media == NULL)
                break;

            if (selected)
                view->select(media);
            else
                view->unselect(media);
        }
    }

    // The other case is when the user is selecting or deselecting media.  We
    // walk around looking for other media to see if we should select or deselect
    // the containing event.
    //
    // TODO: Select/Unselect in bulk operations for efficiency
    foreach (object, *toggled) {
        MediaSource* media = qobject_cast<MediaSource*>(object);
        if (media == NULL)
            break;

        int index = view->indexOf(media);
        bool all_match_selection = true;

        // First walk forward to the next event/end of list looking for differing
        // selection states in other media.
        for (int i = index + 1; i < count; ++i) {
            MediaSource* media = qobject_cast<MediaSource*>(view->getAt(i));
            if (media == NULL)
                break;

            if (view->isSelected(media) != selected) {
                all_match_selection = false;
                break;
            }
        }

        // Now walk backwards also checking media, and looking for the event.
        for (int i = index - 1; i >= 0; --i) {
            MediaSource* media = qobject_cast<MediaSource*>(view->getAt(i));
            Event* event = qobject_cast<Event*>(view->getAt(i));

            if (media != NULL) {
                if (view->isSelected(media) != selected)
                    all_match_selection = false;
            }

            if (event != NULL) {
                if (all_match_selection && selected)
                    view->select(event);
                else
                    view->unselect(event);
                break;
            }
        }
    }

    m_syncingMedia = false;
}

/*!
 * \brief QmlEventOverviewModel::ascendingComparator
 * \param a
 * \param b
 * \return
 */
bool QmlEventOverviewModel::ascendingComparator(DataObject* a, DataObject* b)
{
    return eventComparator(a, b, true);
}

/*!
 * \brief QmlEventOverviewModel::descendingComparator
 * \param a
 * \param b
 * \return
 */
bool QmlEventOverviewModel::descendingComparator(DataObject* a, DataObject* b)
{
    return eventComparator(a, b, false);
}

/*!
 * \brief QmlEventOverviewModel::eventComparator
 * \param a
 * \param b
 * \param asc
 * \return
 */
bool QmlEventOverviewModel::eventComparator(DataObject* a, DataObject* b, bool asc)
{
    QDateTime atime = objectDateTime(a, asc);
    QDateTime btime = objectDateTime(b, asc);

    // use default comparator to stabilize order (reversing comparison for descending
    // order)
    bool lessThan;
    if (atime != btime)
        lessThan = atime < btime;
    else
        lessThan = DataCollection::defaultDataObjectComparator(a, b);

    return (asc) ? lessThan : !lessThan;
}

/*!
 * \brief QmlEventOverviewModel::objectDateTime
 * Since items in the list can be either a MediaSource or an Event,
 * determine dynamically and compare.  Since going in reverse chronological order,
 * use the event's end date/time for comparison (to place it before everything
 * else inside of it)
 * \param object
 * \param asc
 * \return
 */
QDateTime QmlEventOverviewModel::objectDateTime(DataObject* object, bool asc)
{
    MediaSource* media = qobject_cast<MediaSource*>(object);
    if (media != NULL)
        return media->exposureDateTime();

    // Events are different depending on ascending vs. descending; they should
    // always be at the head of the MediaSource span, so they need to use different
    // times to ensure that
    Event* event = qobject_cast<Event*>(object);
    if (event != NULL)
        return asc ? event->startDateTime() : event->endDateTime();

    return QDateTime();
}

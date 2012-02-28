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

#include "qml/qml-event-overview-model.h"

#include "core/data-collection.h"
#include "event/event-collection.h"
#include "media/media-source.h"
#include "qml/qml-event-marker.h"
#include "util/variants.h"

QmlEventOverviewModel::QmlEventOverviewModel(QObject* parent)
  : QmlMediaCollectionModel(parent, Comparator), markers_("QmlEventMarkers") {
  // initialize ViewCollection as it stands now with EventMarkers
  MonitorNewViewCollection();
}

QmlEventOverviewModel::~QmlEventOverviewModel() {
  markers_.DestroyAll(false, true);
}

void QmlEventOverviewModel::RegisterType() {
  qmlRegisterType<QmlEventOverviewModel>("Gallery", 1, 0, "EventOverviewModel");
}

QVariant QmlEventOverviewModel::VariantFor(DataObject* object) const {
  QmlEventMarker* marker = qobject_cast<QmlEventMarker*>(object);
  
  return (marker != NULL) ? QVariant::fromValue(marker) : QmlMediaCollectionModel::VariantFor(object);
}

DataObject* QmlEventOverviewModel::FromVariant(QVariant var) const {
  DataObject* object = UncheckedVariantToObject<QmlEventMarker*>(var);
  
  return (object != NULL) ? object : QmlMediaCollectionModel::FromVariant(var);
}

void QmlEventOverviewModel::notify_backing_collection_changed() {
  MonitorNewViewCollection();
  
  QmlViewCollectionModel::notify_backing_collection_changed();
}

void QmlEventOverviewModel::MonitorNewViewCollection() {
  if (BackingViewCollection() == NULL)
    return;
  
  // destroy existing markers
  markers_.DestroyAll(true, true);
  
  QObject::connect(
    BackingViewCollection(),
    SIGNAL(contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_event_overview_contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  QObject::connect(
    BackingViewCollection(),
    SIGNAL(selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_event_overview_selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  // seed existing contents with EventMarkers
  on_event_overview_contents_altered(&BackingViewCollection()->GetAsSet(), NULL);
}

void QmlEventOverviewModel::on_event_overview_contents_altered(
  const QSet<DataObject*>* added, const QSet<DataObject*>* removed) {
  SelectableViewCollection* view = BackingViewCollection();
  
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added) {
      MediaSource* source = qobject_cast<MediaSource*>(object);
      if (source == NULL)
        continue;
      
      QDate source_date = source->exposure_date_time().date();
      Event* event = EventCollection::instance()->EventForDate(source_date);
      Q_ASSERT(event != NULL);
      
      int index = view->IndexOf(source);
      if (index == 0) {
        // place EventMarker for first photo
        QmlEventMarker* marker = new QmlEventMarker(event);
        markers_.Add(marker);
        
        view->Add(marker);
        
        continue;
      }
      
      MediaSource* earlier = qobject_cast<MediaSource*>(view->GetAt(index - 1));
      if (earlier == NULL)
        continue;
      
      // if immediately prior photo is of a different day, place an EventMarker
      if (earlier->exposure_date_time().date() != source_date) {
        QmlEventMarker* marker = new QmlEventMarker(event);
        markers_.Add(marker);
        
        view->Add(marker);
      }
    }
  }
  
  // TODO: Remove EventMarkers as all photos for them are removed
  if (removed != NULL) {
  }
}

void QmlEventOverviewModel::on_event_overview_selection_altered(
  const QSet<DataObject*>* selected, const QSet<DataObject*>* unselected) {
  // if an EventMarker is selected, select all photos in that date
  SelectUnselectEvent(selected, true);
  
  // if an EventMarker is unselected, unselect all photos in that date
  SelectUnselectEvent(unselected, false);
}

void QmlEventOverviewModel::SelectUnselectEvent(const QSet<DataObject*>* toggled,
  bool doSelect) {
  if (toggled == NULL)
    return;
  
  SelectableViewCollection* view = BackingViewCollection();
  int count = view->Count();
  
  // Walk the toggle group looking for EventMarker's; when found, walk all the
  // MediaSources that follow and select or unselect them; when another
  // EventMarker is found (or end of list), exit
  //
  // Note that this signal is not reentrant because the list is searched only
  // for EventMarkers and only toggles MediaSources
  //
  // TODO: Select/Unselect in bulk operations for efficiency
  DataObject* object;
  foreach (object, *toggled) {
    QmlEventMarker* marker = qobject_cast<QmlEventMarker*>(object);
    if (marker == NULL)
      continue;
    
    int index = view->IndexOf(marker);
    for (int ctr = index + 1; ctr < count; ctr++) {
      MediaSource* media = qobject_cast<MediaSource*>(view->GetAt(ctr));
      if (media == NULL)
        break;
      
      if (doSelect)
        view->Select(media);
      else
        view->Unselect(media);
    }
  }
}

bool QmlEventOverviewModel::Comparator(DataObject* a, DataObject* b) {
  QDateTime atime = ObjectDateTime(a);
  QDateTime btime = ObjectDateTime(b);
  
  // Sort in reverse chronological order (hence reversed comparison); also,
  // use default comparator to stabilize sort
  return (atime != btime) ? btime < atime : DataCollection::DefaultDataObjectComparator(a, b);
}

// Since items in the list can be either a MediaSource or an EventMarker,
// determine dynamically and compare.  Since going in reverse chronological order,
// use the event's end date/time for comparison (to place it before everything
// else inside of it)
QDateTime QmlEventOverviewModel::ObjectDateTime(DataObject* object) {
  MediaSource* media = qobject_cast<MediaSource*>(object);
  if (media != NULL)
    return media->exposure_date_time();
  
  QmlEventMarker* marker = qobject_cast<QmlEventMarker*>(object);
  if (marker != NULL)
    return marker->end_date_time();
  
  return QDateTime();
}

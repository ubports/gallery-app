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

#include "media/media-source.h"
#include "qml/qml-event-marker.h"

QmlEventOverviewModel::QmlEventOverviewModel(QObject* parent)
  : QmlMediaCollectionModel(parent), markers_("QmlEventMarkers") {
  // Use internal comparator to ensure EventMarkers are in the right place
  SetDefaultComparator(Comparator);
  
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
  if (marker != NULL)
    return QVariant::fromValue(marker);
  
  return QmlMediaCollectionModel::VariantFor(object);
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
    SLOT(on_contents_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  QObject::connect(
    BackingViewCollection(),
    SIGNAL(selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)),
    this,
    SLOT(on_selection_altered(const QSet<DataObject*>*,const QSet<DataObject*>*)));
  
  // seed existing contents with EventMarkers
  on_contents_altered(&BackingViewCollection()->GetAsSet(), NULL);
}

void QmlEventOverviewModel::on_contents_altered(const QSet<DataObject*>* added,
  const QSet<DataObject*>* removed) {
  SelectableViewCollection* view = BackingViewCollection();
  
  if (added != NULL) {
    DataObject* object;
    foreach (object, *added) {
      MediaSource* source = qobject_cast<MediaSource*>(object);
      if (source == NULL)
        continue;
      
      QDate source_date = source->exposure_date_time().date();
      
      int index = view->IndexOf(source);
      if (index == 0) {
        // place EventMarker for first photo
        QmlEventMarker* marker = new QmlEventMarker(source_date);
        markers_.Add(marker);
        
        view->Add(marker);
        
        continue;
      }
      
      MediaSource* earlier = qobject_cast<MediaSource*>(view->GetAt(index - 1));
      if (earlier == NULL)
        continue;
      
      // if immediately prior photo is of a different day, place an EventMarker
      if (earlier->exposure_date_time().date() != source_date) {
        QmlEventMarker* marker = new QmlEventMarker(source_date);
        markers_.Add(marker);
        
        view->Add(marker);
      }
    }
  }
  
  // TODO: Remove EventMarkers as all photos for them are removed
  if (removed != NULL) {
  }
}

void QmlEventOverviewModel::on_selection_altered(const QSet<DataObject*>* selected,
  const QSet<DataObject*>* unselected) {
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
  // Note that this signal is non-reentrant because the list is searched only
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
  return ObjectDateTime(a) < ObjectDateTime(b);
}

QDateTime QmlEventOverviewModel::ObjectDateTime(DataObject* object) {
  MediaSource* media = qobject_cast<MediaSource*>(object);
  if (media != NULL)
    return media->exposure_date_time();
  
  QmlEventMarker* marker = qobject_cast<QmlEventMarker*>(object);
  if (marker != NULL)
    return marker->date_time();
  
  return QDateTime();
}

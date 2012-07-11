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
 * Lucas Beeler <lucas@yorba.org>
 */

#include "qml/qml-media-collection-model.h"

#include "album/album.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "media/media-source.h"
#include "media/media-collection.h"
#include "util/collections.h"
#include "util/variants.h"

QmlMediaCollectionModel::QmlMediaCollectionModel(QObject* parent)
  : QmlViewCollectionModel(parent, "mediaSource", NULL) {
}

QmlMediaCollectionModel::QmlMediaCollectionModel(QObject* parent,
  DataObjectComparator default_comparator)
  : QmlViewCollectionModel(parent, "mediaSource", default_comparator) {
}

void QmlMediaCollectionModel::RegisterType() {
  qmlRegisterType<QmlMediaCollectionModel>("Gallery", 1, 0, "MediaCollectionModel");
}

QVariant QmlMediaCollectionModel::createAlbumFromSelected() {
  SelectableViewCollection* view = BackingViewCollection();
  if (view->SelectedCount() == 0)
    return QVariant();
  
  Album* album = new Album(AlbumDefaultTemplate::instance());
  
  // Attach only the MediaSources in the backing collection
  album->AttachMany(FilterSetOnlyType<DataObject*, MediaSource*>(view->GetSelected()));
  
  AlbumCollection::instance()->Add(album);
  
  return QVariant::fromValue(album);
}

void QmlMediaCollectionModel::destroySelectedMedia() {
  SelectableViewCollection* view = BackingViewCollection();
  if (view->SelectedCount() == 0)
    return;

  MediaCollection::instance()->DestroyMany(
    FilterSetOnlyType<DataObject*, MediaSource*>(view->GetSelected()),
    true, true);
}

void QmlMediaCollectionModel::destroyMedia(QVariant vmedia) {
  MediaSource* media = VariantToObject<MediaSource*>(vmedia);

  if (media != NULL)
    MediaCollection::instance()->Destroy(media, true, true);
}

bool QmlMediaCollectionModel::monitored() const {
  return IsMonitoring();
}

void QmlMediaCollectionModel::set_monitored(bool monitor) {
  if (IsMonitoring() == monitor)
    return;
  
  if (monitor)
    MonitorSourceCollection(MediaCollection::instance());
  else
    StopMonitoring();
  
  monitoring_changed();
}

QVariant QmlMediaCollectionModel::VariantFor(DataObject* object) const {
  MediaSource* media_source = qobject_cast<MediaSource*>(object);
  
  return (media_source != NULL) ? QVariant::fromValue(media_source) : QVariant();
}

DataObject* QmlMediaCollectionModel::FromVariant(QVariant var) const {
  return UncheckedVariantToObject<MediaSource*>(var);
}

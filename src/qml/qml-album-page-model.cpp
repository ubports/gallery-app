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

#include "qml/qml-album-page-model.h"

#include "album/album-page.h"
#include "qml/qml-album-cover-marker.h"
#include "util/variants.h"

QmlAlbumPageModel::QmlAlbumPageModel(QObject* parent)
  : QmlViewCollectionModel(parent, "albumPage", Comparator), album_(NULL),
  include_cover_(false) {
}

void QmlAlbumPageModel::RegisterType() {
  qmlRegisterType<QmlAlbumPageModel>("Gallery", 1, 0, "AlbumPageModel");
}

QVariant QmlAlbumPageModel::VariantFor(DataObject* object) const {
  AlbumPage* album_page = qobject_cast<AlbumPage*>(object);
  
  return (album_page != NULL) ? QVariant::fromValue(album_page) : QVariant();
}

QVariant QmlAlbumPageModel::for_album() const {
  return (album_ != NULL ) ? QVariant::fromValue(album_) : QVariant();
}

void QmlAlbumPageModel::set_for_album(QVariant valbum) {
  Album* album = UncheckedVariantToObject<Album*>(valbum);
  if (album == NULL) {
    StopMonitoring();
    
    return;
  }
  
  if (album_ == album)
    return;
  
  album_ = album;
  MonitorSourceCollection(album_->pages());
  
  if (include_cover_)
    BackingViewCollection()->Add(new QmlAlbumCoverMarker());
  
  album_changed();
}

bool QmlAlbumPageModel::include_cover() const {
  return include_cover_;
}

void QmlAlbumPageModel::set_include_cover(bool include_cover) {
  if (include_cover_ == include_cover)
    return;
  
  include_cover_ = include_cover;
  
  if (IsMonitoring()) {
    if (include_cover_) {
      Q_ASSERT(BackingViewCollection()->GetAtAsType<QmlAlbumCoverMarker*>(0) == NULL);
      BackingViewCollection()->Add(new QmlAlbumCoverMarker());
    } else {
      Q_ASSERT(BackingViewCollection()->GetAtAsType<QmlAlbumCoverMarker*>(0) != NULL);
      BackingViewCollection()->RemoveAt(0);
    }
  }
  
  include_cover_changed();
}

// Ensure that the album cover model is always at the head of the list
bool QmlAlbumPageModel::Comparator(DataObject* a, DataObject* b) {
  if (qobject_cast<QmlAlbumCoverMarker*>(a) != NULL)
    return true;
  
  if (qobject_cast<QmlAlbumCoverMarker*>(b) != NULL)
    return false;
  
  AlbumPage* pagea = qobject_cast<AlbumPage*>(a);
  AlbumPage* pageb = qobject_cast<AlbumPage*>(b);
  
  return pagea->page_number() < pageb->page_number();
}

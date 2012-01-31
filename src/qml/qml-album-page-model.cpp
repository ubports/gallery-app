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
#include "util/variants.h"

QmlAlbumPageModel::QmlAlbumPageModel(QObject* parent)
  : QmlViewCollectionModel(parent, "albumPage", NULL), album_(NULL) {
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
  
  album_ = album;
  MonitorSourceCollection(album_->pages());
  
  album_changed();
}

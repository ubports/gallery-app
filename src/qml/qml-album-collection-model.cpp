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

#include "qml/qml-album-collection-model.h"

#include "album/album.h"
#include "album/album-collection.h"
#include "album/album-default-template.h"
#include "core/utils.h"
#include "media/media-source.h"
#include "qml/qml-media-collection-model.h"

QmlAlbumCollectionModel::QmlAlbumCollectionModel(QObject* parent)
  : QmlViewCollectionModel(parent, "album") {
  MonitorSourceCollection(AlbumCollection::instance());
}

void QmlAlbumCollectionModel::RegisterType() {
  qmlRegisterType<QmlAlbumCollectionModel>("Gallery", 1, 0, "AlbumCollectionModel");
}

void QmlAlbumCollectionModel::createAlbum(QVariant vmedia) {
  Album* album = new Album(*AlbumDefaultTemplate::instance());
  album->Attach(VariantToObject<MediaSource*>(vmedia));
  
  AlbumCollection::instance()->Add(album);
}

QVariant QmlAlbumCollectionModel::VariantFor(DataObject* object) const {
  Album* album = qobject_cast<Album*>(object);
  
  return (album != NULL) ? QVariant::fromValue(album) : QVariant();
}

void QmlAlbumCollectionModel::on_album_current_page_contents_altered(Album* album) {
  NotifyElementAltered(BackingViewCollection()->IndexOf(album), SubclassRole);
}

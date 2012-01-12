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

#include "qml/qml-album-model.h"

#include <QHash>
#include <QList>
#include <QtDeclarative>

#include "media/media-source.h"

QmlAlbumModel::QmlAlbumModel(QObject* parent)
  : QmlViewCollectionModel(parent), album_(NULL), view_("QmlAlbumModel ViewCollection") {
}

void QmlAlbumModel::Init(Album* album) {
  album_ = album;
  
  view_.MonitorDataCollection(album_->pages(), NULL, false);
  
  QHash<int, QByteArray> roles;
  roles[QmlViewCollectionModel::ObjectNumberRole] = "object_number";
  roles[QmlViewCollectionModel::SelectionRole] = "is_selected";
  roles[AlbumPageRole] = "page";
  roles[AlbumNameRole] = "albumName";
  
  QmlViewCollectionModel::Init(&view_, roles);
}

void QmlAlbumModel::RegisterType() {
  qmlRegisterType<QmlAlbumModel>("org.yorba.qt.qmlalbummodel", 1, 0,
    "QmlAlbumModel");
}

QVariant QmlAlbumModel::DataForRole(DataObject* object, int role) const {
  AlbumPage* page = qobject_cast<AlbumPage*>(object);
  if (page == NULL)
    return QVariant();
  
  switch (role) {
    case AlbumPageRole:
      return QVariant::fromValue(page);
    
    case AlbumNameRole:
      return QVariant(album_->name());
    
    default:
      return QVariant();
  }
}

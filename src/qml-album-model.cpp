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

#include "qml-album-model.h"

#include <QHash>
#include <QtDeclarative>

#include "album.h"

QmlAlbumModel::QmlAlbumModel(QObject* parent = NULL)
  : QmlViewCollectionModel(parent) {
}

void QmlAlbumModel::RegisterType() {
  qmlRegisterType<QmlAlbumModel>("org.yorba.qt.qmlalbummodel", 1, 0,
    "QmlAlbumModel");
}

void QmlAlbumModel::Init(SelectableViewCollection* view) {
  QHash<int, QByteArray> roles;
  roles[QmlViewCollectionModel::ObjectNumberRole] = "object_number";
  roles[PreviewPathRole] = "preview_path";
  roles[QmlViewCollectionModel::SelectionRole] = "is_selected";
  
  QmlViewCollectionModel::Init(view, roles);
}

QVariant QmlAlbumModel::DataForRole(DataObject *object, int role) const {
  Album* album = qobject_cast<Album*>(object);
  if (album == NULL)
    return QVariant();
  
  switch (role) {
    case PreviewPathRole:
      return QVariant(album->preview_file().absoluteFilePath());
    
    default:
      return QVariant();
  }
}

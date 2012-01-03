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

#include "qml-album-collection-model.h"

#include <QHash>
#include <QtDeclarative>

#include "album.h"
#include "album-collection.h"
#include "media-source.h"
#include "qml-media-source.h"

QmlAlbumCollectionModel::QmlAlbumCollectionModel(QObject* parent)
  : QmlViewCollectionModel(parent) {
}

void QmlAlbumCollectionModel::RegisterType() {
  qmlRegisterType<QmlAlbumCollectionModel>("org.yorba.qt.qmlalbummodel", 1, 0,
    "QmlAlbumModel");
}

void QmlAlbumCollectionModel::Init(SelectableViewCollection* view) {
  QHash<int, QByteArray> roles;
  roles[QmlViewCollectionModel::ObjectNumberRole] = "object_number";
  roles[QmlViewCollectionModel::SelectionRole] = "is_selected";
  roles[NameRole] = "album_name";
  roles[MediaSourceListRole] = "mediaSourceList";
  roles[QmlRcRole] = "qml_rc";
  
  QmlViewCollectionModel::Init(view, roles);
  
  QObject::connect(
    AlbumCollection::instance(), SIGNAL(album_current_page_contents_altered(Album*)),
    this, SLOT(on_album_current_page_contents_altered(Album*)));
}

QVariant QmlAlbumCollectionModel::DataForRole(DataObject *object, int role) const {
  Album* album = qobject_cast<Album*>(object);
  if (album == NULL)
    return QVariant();
  
  switch (role) {
    case NameRole:
      return QVariant(album->name());
    
    case MediaSourceListRole: {
      QList<QVariant> varlist;
      
      DataObject* object;
      foreach (object, album->GetPage(album->current_page())->contained()->GetAll())
        varlist.append(QmlMediaSource::AsVariant(qobject_cast<MediaSource*>(object)));
      
      return QVariant(varlist);
    }
    
    case QmlRcRole:
      return QVariant(album->GetPage(album->current_page())->template_page()->qml_rc());
    
    default:
      return QVariant();
  }
}

void QmlAlbumCollectionModel::on_album_current_page_contents_altered(Album* album) {
  NotifyElementAltered(BackingViewCollection()->IndexOf(album), MediaSourceListRole);
}
